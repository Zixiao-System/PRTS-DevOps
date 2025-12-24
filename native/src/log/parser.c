/**
 * PRTS Native - Log Parser Implementation
 */

#include "prts/log.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct prts_log_parser {
    prts_log_format_t format;
    char timestamp_format[64];
    bool parse_json_fields;

    /* Parsing buffers */
    char* temp_buffer;
    size_t temp_buffer_size;
};

prts_result_t prts_parser_create(
    const prts_parser_config_t* config,
    prts_log_parser_t** parser_out
) {
    if (!config || !parser_out) {
        return PRTS_ERROR_INVALID;
    }

    prts_log_parser_t* parser = calloc(1, sizeof(prts_log_parser_t));
    if (!parser) {
        return PRTS_ERROR_NOMEM;
    }

    parser->format = config->format;
    parser->parse_json_fields = config->parse_json_fields;

    if (config->timestamp_format) {
        strncpy(parser->timestamp_format, config->timestamp_format,
                sizeof(parser->timestamp_format) - 1);
    }

    parser->temp_buffer_size = 4096;
    parser->temp_buffer = malloc(parser->temp_buffer_size);
    if (!parser->temp_buffer) {
        free(parser);
        return PRTS_ERROR_NOMEM;
    }

    *parser_out = parser;
    return PRTS_OK;
}

void prts_parser_destroy(prts_log_parser_t* parser) {
    if (!parser) return;
    free(parser->temp_buffer);
    free(parser);
}

static prts_log_level_t parse_level(const char* str, size_t len) {
    if (len >= 5 && strncasecmp(str, "TRACE", 5) == 0) return PRTS_LOG_TRACE;
    if (len >= 5 && strncasecmp(str, "DEBUG", 5) == 0) return PRTS_LOG_DEBUG;
    if (len >= 4 && strncasecmp(str, "INFO", 4) == 0) return PRTS_LOG_INFO;
    if (len >= 4 && strncasecmp(str, "WARN", 4) == 0) return PRTS_LOG_WARN;
    if (len >= 5 && strncasecmp(str, "ERROR", 5) == 0) return PRTS_LOG_ERROR;
    if (len >= 5 && strncasecmp(str, "FATAL", 5) == 0) return PRTS_LOG_FATAL;
    return PRTS_LOG_INFO;
}

static prts_log_format_t detect_format(const char* line, size_t len) {
    if (len > 0 && line[0] == '{') {
        return PRTS_LOG_FORMAT_JSON;
    }
    if (len > 0 && line[0] == '<') {
        return PRTS_LOG_FORMAT_SYSLOG;
    }
    return PRTS_LOG_FORMAT_TEXT;
}

prts_result_t prts_parser_parse(
    prts_log_parser_t* parser,
    const char* line,
    size_t line_len,
    prts_log_entry_t* entry_out
) {
    if (!parser || !line || !entry_out) {
        return PRTS_ERROR_INVALID;
    }

    memset(entry_out, 0, sizeof(prts_log_entry_t));

    prts_log_format_t format = parser->format;
    if (format == PRTS_LOG_FORMAT_AUTO) {
        format = detect_format(line, line_len);
    }

    entry_out->raw = line;
    entry_out->raw_len = line_len;
    entry_out->level = PRTS_LOG_INFO;
    entry_out->timestamp = 0;

    switch (format) {
        case PRTS_LOG_FORMAT_JSON: {
            /* Simple JSON parsing */
            /* Look for "level" field */
            const char* level_key = strstr(line, "\"level\"");
            if (level_key) {
                const char* colon = strchr(level_key, ':');
                if (colon) {
                    while (*colon && (*colon == ':' || *colon == ' ' || *colon == '"')) colon++;
                    const char* end = colon;
                    while (*end && *end != '"' && *end != ',' && *end != '}') end++;
                    entry_out->level = parse_level(colon, end - colon);
                }
            }

            /* Look for "message" or "msg" field */
            const char* msg_key = strstr(line, "\"message\"");
            if (!msg_key) msg_key = strstr(line, "\"msg\"");
            if (msg_key) {
                const char* colon = strchr(msg_key, ':');
                if (colon) {
                    while (*colon && (*colon == ':' || *colon == ' ')) colon++;
                    if (*colon == '"') {
                        colon++;
                        const char* end = colon;
                        while (*end && *end != '"') end++;
                        entry_out->message = colon;
                        entry_out->message_len = end - colon;
                    }
                }
            }
            break;
        }

        case PRTS_LOG_FORMAT_TEXT:
        default: {
            /* Simple text parsing: [LEVEL] message or LEVEL message */
            const char* p = line;
            size_t remaining = line_len;

            /* Skip timestamp if present */
            while (remaining > 0 && (isdigit(*p) || *p == '-' || *p == ':' ||
                   *p == 'T' || *p == 'Z' || *p == '.' || *p == ' ')) {
                p++;
                remaining--;
            }

            /* Skip whitespace */
            while (remaining > 0 && isspace(*p)) {
                p++;
                remaining--;
            }

            /* Look for level */
            if (*p == '[') {
                p++;
                remaining--;
                const char* level_end = strchr(p, ']');
                if (level_end) {
                    entry_out->level = parse_level(p, level_end - p);
                    p = level_end + 1;
                    remaining = line_len - (p - line);
                }
            } else {
                /* Check for level without brackets */
                const char* levels[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
                for (size_t i = 0; i < 6; i++) {
                    size_t level_len = strlen(levels[i]);
                    if (remaining >= level_len && strncasecmp(p, levels[i], level_len) == 0) {
                        entry_out->level = (prts_log_level_t)i;
                        p += level_len;
                        remaining = line_len - (p - line);
                        break;
                    }
                }
            }

            /* Skip whitespace */
            while (remaining > 0 && isspace(*p)) {
                p++;
                remaining--;
            }

            entry_out->message = p;
            entry_out->message_len = remaining;
            break;
        }
    }

    return PRTS_OK;
}

prts_result_t prts_parser_parse_batch(
    prts_log_parser_t* parser,
    const char* data,
    size_t data_len,
    prts_log_entry_t* entries_out,
    size_t max_entries,
    size_t* count_out
) {
    if (!parser || !data || !entries_out || !count_out) {
        return PRTS_ERROR_INVALID;
    }

    size_t count = 0;
    const char* line_start = data;
    const char* end = data + data_len;

    while (line_start < end && count < max_entries) {
        /* Find line end */
        const char* line_end = line_start;
        while (line_end < end && *line_end != '\n') {
            line_end++;
        }

        /* Skip empty lines */
        if (line_end > line_start) {
            prts_result_t result = prts_parser_parse(
                parser,
                line_start,
                line_end - line_start,
                &entries_out[count]
            );
            if (result == PRTS_OK) {
                count++;
            }
        }

        line_start = line_end + 1;
    }

    *count_out = count;
    return PRTS_OK;
}

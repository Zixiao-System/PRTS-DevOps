/**
 * PRTS Native - Log Parser and Indexer
 * High-performance log parsing and full-text indexing.
 */

#ifndef PRTS_LOG_H
#define PRTS_LOG_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Log level */
typedef enum {
    PRTS_LOG_TRACE = 0,
    PRTS_LOG_DEBUG = 1,
    PRTS_LOG_INFO = 2,
    PRTS_LOG_WARN = 3,
    PRTS_LOG_ERROR = 4,
    PRTS_LOG_FATAL = 5,
} prts_log_level_t;

/* Log format */
typedef enum {
    PRTS_LOG_FORMAT_JSON,
    PRTS_LOG_FORMAT_TEXT,
    PRTS_LOG_FORMAT_SYSLOG,
    PRTS_LOG_FORMAT_AUTO,
} prts_log_format_t;

/* Parsed log entry */
typedef struct {
    prts_timestamp_t timestamp;
    prts_log_level_t level;
    const char* message;
    size_t message_len;
    const char* source;
    size_t source_len;
    const char* raw;
    size_t raw_len;
    /* Extra fields for JSON logs */
    const char** field_names;
    const char** field_values;
    size_t num_fields;
} prts_log_entry_t;

/* Parser configuration */
typedef struct {
    prts_log_format_t format;
    const char* timestamp_format;   /* strptime format, NULL for auto */
    bool parse_json_fields;         /* Extract JSON fields */
} prts_parser_config_t;

/* Indexer configuration */
typedef struct {
    const char* index_path;         /* Path to index directory */
    size_t memory_limit;            /* Memory limit in bytes */
    bool enable_compression;        /* Enable index compression */
    size_t shard_size;              /* Number of entries per shard */
} prts_indexer_config_t;

/* Search query */
typedef struct {
    const char* query;              /* Full-text search query */
    prts_timestamp_t start_time;    /* Start timestamp (0 = no limit) */
    prts_timestamp_t end_time;      /* End timestamp (0 = no limit) */
    prts_log_level_t min_level;     /* Minimum log level */
    const char* source_filter;      /* Source filter pattern */
    size_t offset;                  /* Result offset */
    size_t limit;                   /* Maximum results */
} prts_search_query_t;

/* Search result */
typedef struct {
    prts_log_entry_t* entries;
    size_t count;
    size_t total_matches;
    uint64_t search_time_ns;
} prts_search_result_t;

/* === Log Parser === */

/**
 * Create a log parser.
 * @param config Parser configuration
 * @param parser_out Output pointer for parser
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_parser_create(
    const prts_parser_config_t* config,
    prts_log_parser_t** parser_out
);

/**
 * Destroy a log parser.
 * @param parser The parser to destroy
 */
PRTS_API void prts_parser_destroy(prts_log_parser_t* parser);

/**
 * Parse a single log line.
 * @param parser The log parser
 * @param line Log line to parse
 * @param line_len Length of the log line
 * @param entry_out Output entry (fields are valid until next parse call)
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_parser_parse(
    prts_log_parser_t* parser,
    const char* line,
    size_t line_len,
    prts_log_entry_t* entry_out
);

/**
 * Parse multiple log lines.
 * @param parser The log parser
 * @param data Log data to parse
 * @param data_len Length of the log data
 * @param entries_out Output entries array
 * @param max_entries Maximum entries to parse
 * @param count_out Actual number of entries parsed
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_parser_parse_batch(
    prts_log_parser_t* parser,
    const char* data,
    size_t data_len,
    prts_log_entry_t* entries_out,
    size_t max_entries,
    size_t* count_out
);

/* === Log Indexer === */

/**
 * Create a log indexer.
 * @param config Indexer configuration
 * @param indexer_out Output pointer for indexer
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_indexer_create(
    const prts_indexer_config_t* config,
    prts_log_indexer_t** indexer_out
);

/**
 * Destroy a log indexer.
 * @param indexer The indexer to destroy
 */
PRTS_API void prts_indexer_destroy(prts_log_indexer_t* indexer);

/**
 * Index a log entry.
 * @param indexer The log indexer
 * @param entry Log entry to index
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_indexer_add(
    prts_log_indexer_t* indexer,
    const prts_log_entry_t* entry
);

/**
 * Index multiple log entries.
 * @param indexer The log indexer
 * @param entries Log entries to index
 * @param count Number of entries
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_indexer_add_batch(
    prts_log_indexer_t* indexer,
    const prts_log_entry_t* entries,
    size_t count
);

/**
 * Search the log index.
 * @param indexer The log indexer
 * @param query Search query
 * @param result_out Output search result (caller must free with prts_search_result_free)
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_indexer_search(
    prts_log_indexer_t* indexer,
    const prts_search_query_t* query,
    prts_search_result_t** result_out
);

/**
 * Free a search result.
 * @param result The search result to free
 */
PRTS_API void prts_search_result_free(prts_search_result_t* result);

/**
 * Flush pending writes to disk.
 * @param indexer The log indexer
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_indexer_flush(prts_log_indexer_t* indexer);

/**
 * Compact the index.
 * @param indexer The log indexer
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_indexer_compact(prts_log_indexer_t* indexer);

#ifdef __cplusplus
}
#endif

#endif /* PRTS_LOG_H */

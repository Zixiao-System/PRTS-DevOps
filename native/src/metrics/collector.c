/**
 * PRTS Native - Metrics Collector Implementation
 */

#include "prts/metrics.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <time.h>
#endif

/* Maximum metrics */
#define MAX_METRICS 256
#define MAX_LABEL_VALUES 16
#define MAX_NAME_LEN 128

/* Metric entry */
typedef struct {
    char name[MAX_NAME_LEN];
    char description[256];
    prts_metric_type_t type;
    char** labels;
    size_t num_labels;

    /* Values for different label combinations */
    struct {
        char** label_values;
        union {
            uint64_t counter;
            double gauge;
            struct {
                uint64_t count;
                double sum;
                uint64_t* bucket_counts;
                double* boundaries;
                size_t num_buckets;
            } histogram;
        } value;
    } entries[MAX_LABEL_VALUES];
    size_t num_entries;
} metric_entry_t;

/* Metrics collector structure */
struct prts_metrics_collector {
    metric_entry_t metrics[MAX_METRICS];
    size_t num_metrics;

#ifdef _WIN32
    CRITICAL_SECTION lock;
#else
    pthread_mutex_t lock;
#endif
};

prts_result_t prts_metrics_create(prts_metrics_collector_t** collector_out) {
    if (!collector_out) {
        return PRTS_ERROR_INVALID;
    }

    prts_metrics_collector_t* collector = calloc(1, sizeof(prts_metrics_collector_t));
    if (!collector) {
        return PRTS_ERROR_NOMEM;
    }

#ifdef _WIN32
    InitializeCriticalSection(&collector->lock);
#else
    pthread_mutex_init(&collector->lock, NULL);
#endif

    *collector_out = collector;
    return PRTS_OK;
}

void prts_metrics_destroy(prts_metrics_collector_t* collector) {
    if (!collector) return;

#ifdef _WIN32
    DeleteCriticalSection(&collector->lock);
#else
    pthread_mutex_destroy(&collector->lock);
#endif

    /* Free allocated memory */
    for (size_t i = 0; i < collector->num_metrics; i++) {
        metric_entry_t* m = &collector->metrics[i];
        if (m->labels) {
            for (size_t j = 0; j < m->num_labels; j++) {
                free(m->labels[j]);
            }
            free(m->labels);
        }
    }

    free(collector);
}

static metric_entry_t* find_metric(prts_metrics_collector_t* collector, const char* name) {
    for (size_t i = 0; i < collector->num_metrics; i++) {
        if (strcmp(collector->metrics[i].name, name) == 0) {
            return &collector->metrics[i];
        }
    }
    return NULL;
}

prts_result_t prts_metrics_register(
    prts_metrics_collector_t* collector,
    const prts_metric_config_t* config
) {
    if (!collector || !config || !config->name) {
        return PRTS_ERROR_INVALID;
    }

    if (collector->num_metrics >= MAX_METRICS) {
        return PRTS_ERROR_FULL;
    }

#ifdef _WIN32
    EnterCriticalSection(&collector->lock);
#else
    pthread_mutex_lock(&collector->lock);
#endif

    if (find_metric(collector, config->name)) {
#ifdef _WIN32
        LeaveCriticalSection(&collector->lock);
#else
        pthread_mutex_unlock(&collector->lock);
#endif
        return PRTS_ERROR_INVALID; /* Already exists */
    }

    metric_entry_t* m = &collector->metrics[collector->num_metrics++];
    strncpy(m->name, config->name, MAX_NAME_LEN - 1);
    if (config->description) {
        strncpy(m->description, config->description, sizeof(m->description) - 1);
    }
    m->type = config->type;
    m->num_entries = 0;

    /* Copy labels */
    if (config->labels && config->num_labels > 0) {
        m->labels = calloc(config->num_labels, sizeof(char*));
        m->num_labels = config->num_labels;
        for (size_t i = 0; i < config->num_labels; i++) {
            m->labels[i] = strdup(config->labels[i]);
        }
    }

#ifdef _WIN32
    LeaveCriticalSection(&collector->lock);
#else
    pthread_mutex_unlock(&collector->lock);
#endif

    return PRTS_OK;
}

prts_result_t prts_metrics_counter_inc(
    prts_metrics_collector_t* collector,
    const char* name,
    const char** label_values,
    uint64_t delta
) {
    if (!collector || !name) {
        return PRTS_ERROR_INVALID;
    }

#ifdef _WIN32
    EnterCriticalSection(&collector->lock);
#else
    pthread_mutex_lock(&collector->lock);
#endif

    metric_entry_t* m = find_metric(collector, name);
    if (!m || m->type != PRTS_METRIC_COUNTER) {
#ifdef _WIN32
        LeaveCriticalSection(&collector->lock);
#else
        pthread_mutex_unlock(&collector->lock);
#endif
        return PRTS_ERROR_INVALID;
    }

    /* Find or create entry for label values */
    size_t entry_idx = 0; /* Default entry for no labels */
    if (m->num_entries == 0) {
        m->num_entries = 1;
    }

    m->entries[entry_idx].value.counter += delta;

#ifdef _WIN32
    LeaveCriticalSection(&collector->lock);
#else
    pthread_mutex_unlock(&collector->lock);
#endif

    return PRTS_OK;
}

prts_result_t prts_metrics_gauge_set(
    prts_metrics_collector_t* collector,
    const char* name,
    const char** label_values,
    double value
) {
    if (!collector || !name) {
        return PRTS_ERROR_INVALID;
    }

#ifdef _WIN32
    EnterCriticalSection(&collector->lock);
#else
    pthread_mutex_lock(&collector->lock);
#endif

    metric_entry_t* m = find_metric(collector, name);
    if (!m || m->type != PRTS_METRIC_GAUGE) {
#ifdef _WIN32
        LeaveCriticalSection(&collector->lock);
#else
        pthread_mutex_unlock(&collector->lock);
#endif
        return PRTS_ERROR_INVALID;
    }

    size_t entry_idx = 0;
    if (m->num_entries == 0) {
        m->num_entries = 1;
    }

    m->entries[entry_idx].value.gauge = value;

#ifdef _WIN32
    LeaveCriticalSection(&collector->lock);
#else
    pthread_mutex_unlock(&collector->lock);
#endif

    return PRTS_OK;
}

prts_result_t prts_metrics_histogram_observe(
    prts_metrics_collector_t* collector,
    const char* name,
    const char** label_values,
    double value
) {
    if (!collector || !name) {
        return PRTS_ERROR_INVALID;
    }

#ifdef _WIN32
    EnterCriticalSection(&collector->lock);
#else
    pthread_mutex_lock(&collector->lock);
#endif

    metric_entry_t* m = find_metric(collector, name);
    if (!m || m->type != PRTS_METRIC_HISTOGRAM) {
#ifdef _WIN32
        LeaveCriticalSection(&collector->lock);
#else
        pthread_mutex_unlock(&collector->lock);
#endif
        return PRTS_ERROR_INVALID;
    }

    size_t entry_idx = 0;
    if (m->num_entries == 0) {
        m->num_entries = 1;
    }

    m->entries[entry_idx].value.histogram.count++;
    m->entries[entry_idx].value.histogram.sum += value;

#ifdef _WIN32
    LeaveCriticalSection(&collector->lock);
#else
    pthread_mutex_unlock(&collector->lock);
#endif

    return PRTS_OK;
}

prts_result_t prts_metrics_export_prometheus(
    prts_metrics_collector_t* collector,
    char* buffer,
    size_t buffer_size,
    size_t* written_out
) {
    if (!collector || !buffer || !written_out) {
        return PRTS_ERROR_INVALID;
    }

#ifdef _WIN32
    EnterCriticalSection(&collector->lock);
#else
    pthread_mutex_lock(&collector->lock);
#endif

    size_t offset = 0;

    for (size_t i = 0; i < collector->num_metrics && offset < buffer_size; i++) {
        metric_entry_t* m = &collector->metrics[i];

        /* Write HELP */
        offset += snprintf(buffer + offset, buffer_size - offset,
            "# HELP %s %s\n", m->name, m->description);

        /* Write TYPE */
        const char* type_str = "untyped";
        switch (m->type) {
            case PRTS_METRIC_COUNTER: type_str = "counter"; break;
            case PRTS_METRIC_GAUGE: type_str = "gauge"; break;
            case PRTS_METRIC_HISTOGRAM: type_str = "histogram"; break;
        }
        offset += snprintf(buffer + offset, buffer_size - offset,
            "# TYPE %s %s\n", m->name, type_str);

        /* Write values */
        for (size_t j = 0; j < m->num_entries && offset < buffer_size; j++) {
            switch (m->type) {
                case PRTS_METRIC_COUNTER:
                    offset += snprintf(buffer + offset, buffer_size - offset,
                        "%s %llu\n", m->name,
                        (unsigned long long)m->entries[j].value.counter);
                    break;
                case PRTS_METRIC_GAUGE:
                    offset += snprintf(buffer + offset, buffer_size - offset,
                        "%s %g\n", m->name, m->entries[j].value.gauge);
                    break;
                case PRTS_METRIC_HISTOGRAM:
                    offset += snprintf(buffer + offset, buffer_size - offset,
                        "%s_count %llu\n%s_sum %g\n",
                        m->name, (unsigned long long)m->entries[j].value.histogram.count,
                        m->name, m->entries[j].value.histogram.sum);
                    break;
            }
        }
    }

    *written_out = offset;

#ifdef _WIN32
    LeaveCriticalSection(&collector->lock);
#else
    pthread_mutex_unlock(&collector->lock);
#endif

    return PRTS_OK;
}

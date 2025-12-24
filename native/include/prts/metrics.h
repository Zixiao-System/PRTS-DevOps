/**
 * PRTS Native - Metrics Collector
 * High-performance metrics collection and aggregation.
 */

#ifndef PRTS_METRICS_H
#define PRTS_METRICS_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Metric types */
typedef enum {
    PRTS_METRIC_COUNTER,    /* Monotonically increasing counter */
    PRTS_METRIC_GAUGE,      /* Point-in-time value */
    PRTS_METRIC_HISTOGRAM,  /* Distribution of values */
} prts_metric_type_t;

/* Metric configuration */
typedef struct {
    const char* name;
    const char* description;
    prts_metric_type_t type;
    const char** labels;
    size_t num_labels;
} prts_metric_config_t;

/* Histogram buckets */
typedef struct {
    double* boundaries;
    size_t num_boundaries;
} prts_histogram_config_t;

/* Metric value */
typedef struct {
    prts_metric_type_t type;
    prts_timestamp_t timestamp;
    union {
        uint64_t counter;
        double gauge;
        struct {
            uint64_t count;
            double sum;
            uint64_t* bucket_counts;
            size_t num_buckets;
        } histogram;
    } value;
} prts_metric_value_t;

/**
 * Create a metrics collector.
 * @param collector_out Output pointer for collector
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_metrics_create(
    prts_metrics_collector_t** collector_out
);

/**
 * Destroy a metrics collector.
 * @param collector The collector to destroy
 */
PRTS_API void prts_metrics_destroy(prts_metrics_collector_t* collector);

/**
 * Register a new metric.
 * @param collector The metrics collector
 * @param config Metric configuration
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_metrics_register(
    prts_metrics_collector_t* collector,
    const prts_metric_config_t* config
);

/**
 * Increment a counter metric.
 * @param collector The metrics collector
 * @param name Metric name
 * @param label_values Label values (NULL for no labels)
 * @param delta Increment amount
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_metrics_counter_inc(
    prts_metrics_collector_t* collector,
    const char* name,
    const char** label_values,
    uint64_t delta
);

/**
 * Set a gauge metric.
 * @param collector The metrics collector
 * @param name Metric name
 * @param label_values Label values (NULL for no labels)
 * @param value Gauge value
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_metrics_gauge_set(
    prts_metrics_collector_t* collector,
    const char* name,
    const char** label_values,
    double value
);

/**
 * Observe a histogram value.
 * @param collector The metrics collector
 * @param name Metric name
 * @param label_values Label values (NULL for no labels)
 * @param value Observed value
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_metrics_histogram_observe(
    prts_metrics_collector_t* collector,
    const char* name,
    const char** label_values,
    double value
);

/**
 * Get metric value.
 * @param collector The metrics collector
 * @param name Metric name
 * @param label_values Label values (NULL for no labels)
 * @param value_out Output value
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_metrics_get(
    prts_metrics_collector_t* collector,
    const char* name,
    const char** label_values,
    prts_metric_value_t* value_out
);

/**
 * Export metrics in Prometheus format.
 * @param collector The metrics collector
 * @param buffer Output buffer
 * @param buffer_size Buffer size
 * @param written_out Bytes written
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_metrics_export_prometheus(
    prts_metrics_collector_t* collector,
    char* buffer,
    size_t buffer_size,
    size_t* written_out
);

#ifdef __cplusplus
}
#endif

#endif /* PRTS_METRICS_H */

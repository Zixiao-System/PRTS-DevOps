/**
 * PRTS Native - Core Types
 * Common type definitions used across the native module.
 */

#ifndef PRTS_TYPES_H
#define PRTS_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef _WIN32
    #ifdef PRTS_BUILD_DLL
        #define PRTS_API __declspec(dllexport)
    #else
        #define PRTS_API __declspec(dllimport)
    #endif
#else
    #define PRTS_API __attribute__((visibility("default")))
#endif

/* Result codes */
typedef enum {
    PRTS_OK = 0,
    PRTS_ERROR = -1,
    PRTS_ERROR_NOMEM = -2,
    PRTS_ERROR_INVALID = -3,
    PRTS_ERROR_TIMEOUT = -4,
    PRTS_ERROR_FULL = -5,
    PRTS_ERROR_EMPTY = -6,
} prts_result_t;

/* Timestamp in nanoseconds */
typedef uint64_t prts_timestamp_t;

/* Handle types */
typedef struct prts_memory_pool prts_memory_pool_t;
typedef struct prts_thread_pool prts_thread_pool_t;
typedef struct prts_ring_buffer prts_ring_buffer_t;
typedef struct prts_metrics_collector prts_metrics_collector_t;
typedef struct prts_log_parser prts_log_parser_t;
typedef struct prts_log_indexer prts_log_indexer_t;

/* Callback types */
typedef void (*prts_task_fn)(void* arg);
typedef void (*prts_free_fn)(void* ptr);

/* Get current timestamp in nanoseconds */
PRTS_API prts_timestamp_t prts_timestamp_now(void);

/* Convert timestamp to string */
PRTS_API void prts_timestamp_to_str(prts_timestamp_t ts, char* buf, size_t buf_size);

#endif /* PRTS_TYPES_H */

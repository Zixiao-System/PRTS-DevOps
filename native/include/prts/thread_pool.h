/**
 * PRTS Native - Thread Pool
 * High-performance thread pool for parallel task execution.
 */

#ifndef PRTS_THREAD_POOL_H
#define PRTS_THREAD_POOL_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Thread pool configuration */
typedef struct {
    size_t num_threads;     /* Number of worker threads (0 = auto) */
    size_t queue_size;      /* Task queue size */
    bool allow_grow;        /* Allow dynamic thread growth */
    size_t max_threads;     /* Maximum threads if allow_grow is true */
} prts_threadpool_config_t;

/* Thread pool statistics */
typedef struct {
    size_t active_threads;
    size_t idle_threads;
    size_t pending_tasks;
    size_t completed_tasks;
    uint64_t total_wait_ns;
    uint64_t total_exec_ns;
} prts_threadpool_stats_t;

/* Task handle */
typedef struct prts_task prts_task_t;

/**
 * Create a new thread pool.
 * @param config Pool configuration
 * @param pool_out Output pointer for the pool
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_threadpool_create(
    const prts_threadpool_config_t* config,
    prts_thread_pool_t** pool_out
);

/**
 * Destroy a thread pool.
 * Waits for all pending tasks to complete.
 * @param pool The thread pool
 */
PRTS_API void prts_threadpool_destroy(prts_thread_pool_t* pool);

/**
 * Submit a task to the thread pool.
 * @param pool The thread pool
 * @param fn Task function
 * @param arg Task argument
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_threadpool_submit(
    prts_thread_pool_t* pool,
    prts_task_fn fn,
    void* arg
);

/**
 * Submit a task and get a handle for waiting.
 * @param pool The thread pool
 * @param fn Task function
 * @param arg Task argument
 * @param task_out Output pointer for task handle
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_threadpool_submit_wait(
    prts_thread_pool_t* pool,
    prts_task_fn fn,
    void* arg,
    prts_task_t** task_out
);

/**
 * Wait for a task to complete.
 * @param task The task handle
 * @param timeout_ms Timeout in milliseconds (0 = no wait, -1 = infinite)
 * @return PRTS_OK on success, PRTS_ERROR_TIMEOUT on timeout
 */
PRTS_API prts_result_t prts_task_wait(prts_task_t* task, int timeout_ms);

/**
 * Free a task handle.
 * @param task The task handle
 */
PRTS_API void prts_task_free(prts_task_t* task);

/**
 * Get thread pool statistics.
 * @param pool The thread pool
 * @param stats Output statistics
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_threadpool_stats(
    prts_thread_pool_t* pool,
    prts_threadpool_stats_t* stats
);

/**
 * Wait for all pending tasks to complete.
 * @param pool The thread pool
 */
PRTS_API void prts_threadpool_wait_all(prts_thread_pool_t* pool);

#ifdef __cplusplus
}
#endif

#endif /* PRTS_THREAD_POOL_H */

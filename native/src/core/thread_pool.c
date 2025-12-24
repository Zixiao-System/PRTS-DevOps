/**
 * PRTS Native - Thread Pool Implementation
 */

#include "prts/thread_pool.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

/* Task structure */
typedef struct task_node {
    prts_task_fn fn;
    void* arg;
    struct task_node* next;
    prts_task_t* wait_handle;
} task_node_t;

/* Task wait handle */
struct prts_task {
    bool completed;
#ifdef _WIN32
    HANDLE event;
#else
    pthread_mutex_t mutex;
    pthread_cond_t cond;
#endif
};

/* Thread pool structure */
struct prts_thread_pool {
    size_t num_threads;
    size_t queue_size;
    size_t max_threads;
    bool allow_grow;
    bool shutdown;

#ifdef _WIN32
    HANDLE* threads;
    CRITICAL_SECTION lock;
    CONDITION_VARIABLE not_empty;
    CONDITION_VARIABLE not_full;
#else
    pthread_t* threads;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
#endif

    task_node_t* head;
    task_node_t* tail;
    size_t task_count;

    size_t active_count;
    size_t completed_count;
    uint64_t total_wait_ns;
    uint64_t total_exec_ns;
};

/* Worker thread function */
#ifdef _WIN32
static DWORD WINAPI worker_thread(LPVOID arg)
#else
static void* worker_thread(void* arg)
#endif
{
    prts_thread_pool_t* pool = (prts_thread_pool_t*)arg;

    while (1) {
#ifdef _WIN32
        EnterCriticalSection(&pool->lock);

        while (pool->task_count == 0 && !pool->shutdown) {
            SleepConditionVariableCS(&pool->not_empty, &pool->lock, INFINITE);
        }

        if (pool->shutdown && pool->task_count == 0) {
            LeaveCriticalSection(&pool->lock);
            return 0;
        }
#else
        pthread_mutex_lock(&pool->lock);

        while (pool->task_count == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->not_empty, &pool->lock);
        }

        if (pool->shutdown && pool->task_count == 0) {
            pthread_mutex_unlock(&pool->lock);
            return NULL;
        }
#endif

        /* Dequeue task */
        task_node_t* task = pool->head;
        pool->head = task->next;
        if (!pool->head) {
            pool->tail = NULL;
        }
        pool->task_count--;
        pool->active_count++;

#ifdef _WIN32
        WakeConditionVariable(&pool->not_full);
        LeaveCriticalSection(&pool->lock);
#else
        pthread_cond_signal(&pool->not_full);
        pthread_mutex_unlock(&pool->lock);
#endif

        /* Execute task */
        task->fn(task->arg);

        /* Signal completion */
        if (task->wait_handle) {
            prts_task_t* handle = task->wait_handle;
#ifdef _WIN32
            handle->completed = true;
            SetEvent(handle->event);
#else
            pthread_mutex_lock(&handle->mutex);
            handle->completed = true;
            pthread_cond_signal(&handle->cond);
            pthread_mutex_unlock(&handle->mutex);
#endif
        }

        free(task);

#ifdef _WIN32
        EnterCriticalSection(&pool->lock);
        pool->active_count--;
        pool->completed_count++;
        LeaveCriticalSection(&pool->lock);
#else
        pthread_mutex_lock(&pool->lock);
        pool->active_count--;
        pool->completed_count++;
        pthread_mutex_unlock(&pool->lock);
#endif
    }

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

prts_result_t prts_threadpool_create(
    const prts_threadpool_config_t* config,
    prts_thread_pool_t** pool_out
) {
    if (!config || !pool_out) {
        return PRTS_ERROR_INVALID;
    }

    prts_thread_pool_t* pool = calloc(1, sizeof(prts_thread_pool_t));
    if (!pool) {
        return PRTS_ERROR_NOMEM;
    }

    /* Determine thread count */
    size_t num_threads = config->num_threads;
    if (num_threads == 0) {
#ifdef _WIN32
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        num_threads = sysinfo.dwNumberOfProcessors;
#else
        num_threads = sysconf(_SC_NPROCESSORS_ONLN);
#endif
    }

    pool->num_threads = num_threads;
    pool->queue_size = config->queue_size > 0 ? config->queue_size : 1024;
    pool->max_threads = config->max_threads;
    pool->allow_grow = config->allow_grow;

    /* Initialize synchronization */
#ifdef _WIN32
    InitializeCriticalSection(&pool->lock);
    InitializeConditionVariable(&pool->not_empty);
    InitializeConditionVariable(&pool->not_full);

    pool->threads = calloc(num_threads, sizeof(HANDLE));
#else
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->not_empty, NULL);
    pthread_cond_init(&pool->not_full, NULL);

    pool->threads = calloc(num_threads, sizeof(pthread_t));
#endif

    if (!pool->threads) {
        free(pool);
        return PRTS_ERROR_NOMEM;
    }

    /* Create worker threads */
    for (size_t i = 0; i < num_threads; i++) {
#ifdef _WIN32
        pool->threads[i] = CreateThread(NULL, 0, worker_thread, pool, 0, NULL);
        if (!pool->threads[i]) {
            pool->shutdown = true;
            for (size_t j = 0; j < i; j++) {
                WaitForSingleObject(pool->threads[j], INFINITE);
                CloseHandle(pool->threads[j]);
            }
            free(pool->threads);
            free(pool);
            return PRTS_ERROR;
        }
#else
        if (pthread_create(&pool->threads[i], NULL, worker_thread, pool) != 0) {
            pool->shutdown = true;
            pthread_cond_broadcast(&pool->not_empty);
            for (size_t j = 0; j < i; j++) {
                pthread_join(pool->threads[j], NULL);
            }
            free(pool->threads);
            free(pool);
            return PRTS_ERROR;
        }
#endif
    }

    *pool_out = pool;
    return PRTS_OK;
}

void prts_threadpool_destroy(prts_thread_pool_t* pool) {
    if (!pool) return;

#ifdef _WIN32
    EnterCriticalSection(&pool->lock);
    pool->shutdown = true;
    WakeAllConditionVariable(&pool->not_empty);
    LeaveCriticalSection(&pool->lock);

    for (size_t i = 0; i < pool->num_threads; i++) {
        WaitForSingleObject(pool->threads[i], INFINITE);
        CloseHandle(pool->threads[i]);
    }

    DeleteCriticalSection(&pool->lock);
#else
    pthread_mutex_lock(&pool->lock);
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->not_empty);
    pthread_mutex_unlock(&pool->lock);

    for (size_t i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->not_empty);
    pthread_cond_destroy(&pool->not_full);
#endif

    /* Free remaining tasks */
    while (pool->head) {
        task_node_t* task = pool->head;
        pool->head = task->next;
        free(task);
    }

    free(pool->threads);
    free(pool);
}

prts_result_t prts_threadpool_submit(
    prts_thread_pool_t* pool,
    prts_task_fn fn,
    void* arg
) {
    return prts_threadpool_submit_wait(pool, fn, arg, NULL);
}

prts_result_t prts_threadpool_submit_wait(
    prts_thread_pool_t* pool,
    prts_task_fn fn,
    void* arg,
    prts_task_t** task_out
) {
    if (!pool || !fn) {
        return PRTS_ERROR_INVALID;
    }

    task_node_t* task = malloc(sizeof(task_node_t));
    if (!task) {
        return PRTS_ERROR_NOMEM;
    }

    task->fn = fn;
    task->arg = arg;
    task->next = NULL;
    task->wait_handle = NULL;

    /* Create wait handle if requested */
    if (task_out) {
        prts_task_t* handle = malloc(sizeof(prts_task_t));
        if (!handle) {
            free(task);
            return PRTS_ERROR_NOMEM;
        }
        handle->completed = false;
#ifdef _WIN32
        handle->event = CreateEvent(NULL, TRUE, FALSE, NULL);
#else
        pthread_mutex_init(&handle->mutex, NULL);
        pthread_cond_init(&handle->cond, NULL);
#endif
        task->wait_handle = handle;
        *task_out = handle;
    }

#ifdef _WIN32
    EnterCriticalSection(&pool->lock);

    while (pool->task_count >= pool->queue_size && !pool->shutdown) {
        SleepConditionVariableCS(&pool->not_full, &pool->lock, INFINITE);
    }
#else
    pthread_mutex_lock(&pool->lock);

    while (pool->task_count >= pool->queue_size && !pool->shutdown) {
        pthread_cond_wait(&pool->not_full, &pool->lock);
    }
#endif

    if (pool->shutdown) {
#ifdef _WIN32
        LeaveCriticalSection(&pool->lock);
#else
        pthread_mutex_unlock(&pool->lock);
#endif
        free(task);
        return PRTS_ERROR;
    }

    /* Enqueue task */
    if (pool->tail) {
        pool->tail->next = task;
    } else {
        pool->head = task;
    }
    pool->tail = task;
    pool->task_count++;

#ifdef _WIN32
    WakeConditionVariable(&pool->not_empty);
    LeaveCriticalSection(&pool->lock);
#else
    pthread_cond_signal(&pool->not_empty);
    pthread_mutex_unlock(&pool->lock);
#endif

    return PRTS_OK;
}

prts_result_t prts_task_wait(prts_task_t* task, int timeout_ms) {
    if (!task) {
        return PRTS_ERROR_INVALID;
    }

#ifdef _WIN32
    DWORD result = WaitForSingleObject(task->event,
        timeout_ms < 0 ? INFINITE : (DWORD)timeout_ms);
    return result == WAIT_OBJECT_0 ? PRTS_OK : PRTS_ERROR_TIMEOUT;
#else
    pthread_mutex_lock(&task->mutex);

    if (timeout_ms < 0) {
        while (!task->completed) {
            pthread_cond_wait(&task->cond, &task->mutex);
        }
    } else if (timeout_ms > 0) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }

        while (!task->completed) {
            if (pthread_cond_timedwait(&task->cond, &task->mutex, &ts) != 0) {
                pthread_mutex_unlock(&task->mutex);
                return PRTS_ERROR_TIMEOUT;
            }
        }
    }

    bool completed = task->completed;
    pthread_mutex_unlock(&task->mutex);

    return completed ? PRTS_OK : PRTS_ERROR_TIMEOUT;
#endif
}

void prts_task_free(prts_task_t* task) {
    if (!task) return;

#ifdef _WIN32
    CloseHandle(task->event);
#else
    pthread_mutex_destroy(&task->mutex);
    pthread_cond_destroy(&task->cond);
#endif

    free(task);
}

prts_result_t prts_threadpool_stats(
    prts_thread_pool_t* pool,
    prts_threadpool_stats_t* stats
) {
    if (!pool || !stats) {
        return PRTS_ERROR_INVALID;
    }

#ifdef _WIN32
    EnterCriticalSection(&pool->lock);
#else
    pthread_mutex_lock(&pool->lock);
#endif

    stats->active_threads = pool->active_count;
    stats->idle_threads = pool->num_threads - pool->active_count;
    stats->pending_tasks = pool->task_count;
    stats->completed_tasks = pool->completed_count;
    stats->total_wait_ns = pool->total_wait_ns;
    stats->total_exec_ns = pool->total_exec_ns;

#ifdef _WIN32
    LeaveCriticalSection(&pool->lock);
#else
    pthread_mutex_unlock(&pool->lock);
#endif

    return PRTS_OK;
}

void prts_threadpool_wait_all(prts_thread_pool_t* pool) {
    if (!pool) return;

#ifdef _WIN32
    EnterCriticalSection(&pool->lock);
    while (pool->task_count > 0 || pool->active_count > 0) {
        SleepConditionVariableCS(&pool->not_full, &pool->lock, INFINITE);
    }
    LeaveCriticalSection(&pool->lock);
#else
    pthread_mutex_lock(&pool->lock);
    while (pool->task_count > 0 || pool->active_count > 0) {
        pthread_cond_wait(&pool->not_full, &pool->lock);
    }
    pthread_mutex_unlock(&pool->lock);
#endif
}

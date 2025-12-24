/**
 * PRTS Native - Memory Pool Implementation
 */

#include "prts/memory_pool.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

/* Block header */
typedef struct block_header {
    struct block_header* next;
} block_header_t;

/* Memory pool structure */
struct prts_memory_pool {
    size_t block_size;
    size_t initial_blocks;
    size_t max_blocks;
    bool thread_safe;

    block_header_t* free_list;
    void* memory;
    size_t memory_size;

    size_t total_blocks;
    size_t used_blocks;
    uint64_t alloc_count;
    uint64_t free_count;

#ifdef _WIN32
    CRITICAL_SECTION lock;
#else
    pthread_mutex_t lock;
#endif
};

/* Align size to 8 bytes */
static size_t align_size(size_t size) {
    return (size + 7) & ~7;
}

prts_result_t prts_pool_create(
    const prts_pool_config_t* config,
    prts_memory_pool_t** pool_out
) {
    if (!config || !pool_out || config->block_size == 0) {
        return PRTS_ERROR_INVALID;
    }

    prts_memory_pool_t* pool = calloc(1, sizeof(prts_memory_pool_t));
    if (!pool) {
        return PRTS_ERROR_NOMEM;
    }

    size_t aligned_size = align_size(config->block_size + sizeof(block_header_t));
    pool->block_size = config->block_size;
    pool->initial_blocks = config->initial_blocks > 0 ? config->initial_blocks : 64;
    pool->max_blocks = config->max_blocks;
    pool->thread_safe = config->thread_safe;

    /* Allocate initial memory */
    pool->memory_size = aligned_size * pool->initial_blocks;
    pool->memory = malloc(pool->memory_size);
    if (!pool->memory) {
        free(pool);
        return PRTS_ERROR_NOMEM;
    }

    /* Initialize free list */
    char* ptr = (char*)pool->memory;
    pool->free_list = NULL;
    for (size_t i = 0; i < pool->initial_blocks; i++) {
        block_header_t* block = (block_header_t*)ptr;
        block->next = pool->free_list;
        pool->free_list = block;
        ptr += aligned_size;
    }
    pool->total_blocks = pool->initial_blocks;

    /* Initialize lock */
    if (pool->thread_safe) {
#ifdef _WIN32
        InitializeCriticalSection(&pool->lock);
#else
        pthread_mutex_init(&pool->lock, NULL);
#endif
    }

    *pool_out = pool;
    return PRTS_OK;
}

void prts_pool_destroy(prts_memory_pool_t* pool) {
    if (!pool) return;

    if (pool->thread_safe) {
#ifdef _WIN32
        DeleteCriticalSection(&pool->lock);
#else
        pthread_mutex_destroy(&pool->lock);
#endif
    }

    free(pool->memory);
    free(pool);
}

static void pool_lock(prts_memory_pool_t* pool) {
    if (pool->thread_safe) {
#ifdef _WIN32
        EnterCriticalSection(&pool->lock);
#else
        pthread_mutex_lock(&pool->lock);
#endif
    }
}

static void pool_unlock(prts_memory_pool_t* pool) {
    if (pool->thread_safe) {
#ifdef _WIN32
        LeaveCriticalSection(&pool->lock);
#else
        pthread_mutex_unlock(&pool->lock);
#endif
    }
}

void* prts_pool_alloc(prts_memory_pool_t* pool) {
    if (!pool) return NULL;

    pool_lock(pool);

    if (!pool->free_list) {
        /* Check if we can grow */
        if (pool->max_blocks > 0 && pool->total_blocks >= pool->max_blocks) {
            pool_unlock(pool);
            return NULL;
        }

        /* Allocate new block from system */
        size_t aligned_size = align_size(pool->block_size + sizeof(block_header_t));
        block_header_t* new_block = malloc(aligned_size);
        if (!new_block) {
            pool_unlock(pool);
            return NULL;
        }

        new_block->next = NULL;
        pool->free_list = new_block;
        pool->total_blocks++;
    }

    block_header_t* block = pool->free_list;
    pool->free_list = block->next;
    pool->used_blocks++;
    pool->alloc_count++;

    pool_unlock(pool);

    /* Return pointer after header */
    return (char*)block + sizeof(block_header_t);
}

void prts_pool_free(prts_memory_pool_t* pool, void* ptr) {
    if (!pool || !ptr) return;

    /* Get header */
    block_header_t* block = (block_header_t*)((char*)ptr - sizeof(block_header_t));

    pool_lock(pool);

    block->next = pool->free_list;
    pool->free_list = block;
    pool->used_blocks--;
    pool->free_count++;

    pool_unlock(pool);
}

prts_result_t prts_pool_stats(
    prts_memory_pool_t* pool,
    prts_pool_stats_t* stats
) {
    if (!pool || !stats) {
        return PRTS_ERROR_INVALID;
    }

    pool_lock(pool);

    stats->total_blocks = pool->total_blocks;
    stats->used_blocks = pool->used_blocks;
    stats->free_blocks = pool->total_blocks - pool->used_blocks;
    stats->total_bytes = pool->total_blocks * pool->block_size;
    stats->used_bytes = pool->used_blocks * pool->block_size;
    stats->alloc_count = pool->alloc_count;
    stats->free_count = pool->free_count;

    pool_unlock(pool);

    return PRTS_OK;
}

void prts_pool_reset(prts_memory_pool_t* pool) {
    if (!pool) return;

    pool_lock(pool);

    /* Rebuild free list from initial memory */
    size_t aligned_size = align_size(pool->block_size + sizeof(block_header_t));
    char* ptr = (char*)pool->memory;
    pool->free_list = NULL;

    for (size_t i = 0; i < pool->initial_blocks; i++) {
        block_header_t* block = (block_header_t*)ptr;
        block->next = pool->free_list;
        pool->free_list = block;
        ptr += aligned_size;
    }

    pool->total_blocks = pool->initial_blocks;
    pool->used_blocks = 0;

    pool_unlock(pool);
}

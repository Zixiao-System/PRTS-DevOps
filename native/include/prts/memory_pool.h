/**
 * PRTS Native - Memory Pool
 * High-performance memory pool for reducing allocation overhead.
 */

#ifndef PRTS_MEMORY_POOL_H
#define PRTS_MEMORY_POOL_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Memory pool configuration */
typedef struct {
    size_t block_size;      /* Size of each block */
    size_t initial_blocks;  /* Initial number of blocks */
    size_t max_blocks;      /* Maximum number of blocks (0 = unlimited) */
    bool thread_safe;       /* Enable thread safety */
} prts_pool_config_t;

/* Pool statistics */
typedef struct {
    size_t total_blocks;
    size_t used_blocks;
    size_t free_blocks;
    size_t total_bytes;
    size_t used_bytes;
    uint64_t alloc_count;
    uint64_t free_count;
} prts_pool_stats_t;

/**
 * Create a new memory pool.
 * @param config Pool configuration
 * @param pool_out Output pointer for the pool
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_pool_create(
    const prts_pool_config_t* config,
    prts_memory_pool_t** pool_out
);

/**
 * Destroy a memory pool.
 * @param pool The pool to destroy
 */
PRTS_API void prts_pool_destroy(prts_memory_pool_t* pool);

/**
 * Allocate a block from the pool.
 * @param pool The memory pool
 * @return Pointer to allocated block, or NULL on failure
 */
PRTS_API void* prts_pool_alloc(prts_memory_pool_t* pool);

/**
 * Free a block back to the pool.
 * @param pool The memory pool
 * @param ptr Pointer to the block
 */
PRTS_API void prts_pool_free(prts_memory_pool_t* pool, void* ptr);

/**
 * Get pool statistics.
 * @param pool The memory pool
 * @param stats Output statistics
 * @return PRTS_OK on success
 */
PRTS_API prts_result_t prts_pool_stats(
    prts_memory_pool_t* pool,
    prts_pool_stats_t* stats
);

/**
 * Reset the pool, freeing all allocations.
 * @param pool The memory pool
 */
PRTS_API void prts_pool_reset(prts_memory_pool_t* pool);

#ifdef __cplusplus
}
#endif

#endif /* PRTS_MEMORY_POOL_H */

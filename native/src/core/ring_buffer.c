/**
 * PRTS Native - Ring Buffer Implementation
 * Lock-free single producer, single consumer ring buffer.
 */

#include "prts/types.h"
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

typedef struct prts_ring_buffer {
    char* buffer;
    size_t capacity;
    size_t element_size;
    atomic_size_t head;
    atomic_size_t tail;
} prts_ring_buffer_t;

prts_result_t prts_ringbuf_create(
    size_t capacity,
    size_t element_size,
    prts_ring_buffer_t** buf_out
) {
    if (!buf_out || capacity == 0 || element_size == 0) {
        return PRTS_ERROR_INVALID;
    }

    /* Round capacity to power of 2 for fast modulo */
    size_t actual_capacity = 1;
    while (actual_capacity < capacity) {
        actual_capacity <<= 1;
    }

    prts_ring_buffer_t* buf = malloc(sizeof(prts_ring_buffer_t));
    if (!buf) {
        return PRTS_ERROR_NOMEM;
    }

    buf->buffer = malloc(actual_capacity * element_size);
    if (!buf->buffer) {
        free(buf);
        return PRTS_ERROR_NOMEM;
    }

    buf->capacity = actual_capacity;
    buf->element_size = element_size;
    atomic_init(&buf->head, 0);
    atomic_init(&buf->tail, 0);

    *buf_out = buf;
    return PRTS_OK;
}

void prts_ringbuf_destroy(prts_ring_buffer_t* buf) {
    if (!buf) return;
    free(buf->buffer);
    free(buf);
}

prts_result_t prts_ringbuf_push(prts_ring_buffer_t* buf, const void* element) {
    if (!buf || !element) {
        return PRTS_ERROR_INVALID;
    }

    size_t head = atomic_load_explicit(&buf->head, memory_order_relaxed);
    size_t next_head = (head + 1) & (buf->capacity - 1);

    if (next_head == atomic_load_explicit(&buf->tail, memory_order_acquire)) {
        return PRTS_ERROR_FULL;
    }

    memcpy(buf->buffer + head * buf->element_size, element, buf->element_size);
    atomic_store_explicit(&buf->head, next_head, memory_order_release);

    return PRTS_OK;
}

prts_result_t prts_ringbuf_pop(prts_ring_buffer_t* buf, void* element_out) {
    if (!buf || !element_out) {
        return PRTS_ERROR_INVALID;
    }

    size_t tail = atomic_load_explicit(&buf->tail, memory_order_relaxed);

    if (tail == atomic_load_explicit(&buf->head, memory_order_acquire)) {
        return PRTS_ERROR_EMPTY;
    }

    memcpy(element_out, buf->buffer + tail * buf->element_size, buf->element_size);
    atomic_store_explicit(&buf->tail, (tail + 1) & (buf->capacity - 1), memory_order_release);

    return PRTS_OK;
}

size_t prts_ringbuf_size(prts_ring_buffer_t* buf) {
    if (!buf) return 0;

    size_t head = atomic_load_explicit(&buf->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&buf->tail, memory_order_acquire);

    return (head - tail) & (buf->capacity - 1);
}

bool prts_ringbuf_empty(prts_ring_buffer_t* buf) {
    if (!buf) return true;

    return atomic_load_explicit(&buf->head, memory_order_acquire) ==
           atomic_load_explicit(&buf->tail, memory_order_acquire);
}

bool prts_ringbuf_full(prts_ring_buffer_t* buf) {
    if (!buf) return true;

    size_t head = atomic_load_explicit(&buf->head, memory_order_acquire);
    size_t tail = atomic_load_explicit(&buf->tail, memory_order_acquire);

    return ((head + 1) & (buf->capacity - 1)) == tail;
}

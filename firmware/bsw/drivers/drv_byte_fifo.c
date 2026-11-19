#include "drv_byte_fifo.h"

#include <stddef.h>
#include <stdint.h>

static uint32_t saturated_add(uint32_t a, uint32_t b)
{
    return (UINT32_MAX - a < b) ? UINT32_MAX : a + b;
}

bs_status_t drv_byte_fifo_init(drv_byte_fifo_t *fifo,
                               uint8_t *storage, uint16_t capacity)
{
    if ((fifo == NULL) || (storage == NULL) || (capacity == 0U)) {
        return BS_STATUS_INVALID_ARGUMENT;
    }

    fifo->initialized = false;
    fifo->storage = storage;
    fifo->capacity = capacity;
    fifo->head = 0U;
    fifo->tail = 0U;
    fifo->used = 0U;
    fifo->dropped_bytes = 0U;
    fifo->initialized = true;
    return BS_STATUS_OK;
}

bs_status_t drv_byte_fifo_write_all(drv_byte_fifo_t *fifo,
                                    const uint8_t *src, uint16_t size)
{
    uint16_t i;

    if ((fifo == NULL) || (src == NULL) || (size == 0U)) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!fifo->initialized || (fifo->storage == NULL) ||
        (fifo->capacity == 0U)) {
        return BS_STATUS_NOT_INITIALIZED;
    }

    if (size > (uint16_t)(fifo->capacity - fifo->used)) {
        fifo->dropped_bytes = saturated_add(fifo->dropped_bytes, size);
        return BS_STATUS_NO_RESOURCES;
    }

    /* 内部复制后立即释放调用方数据所有权；无需动态内存。 */
    for (i = 0U; i < size; ++i) {
        fifo->storage[fifo->tail] = src[i];
        ++fifo->tail;
        if (fifo->tail == fifo->capacity) {
            fifo->tail = 0U;
        }
    }
    fifo->used = (uint16_t)(fifo->used + size);
    return BS_STATUS_OK;
}

bs_status_t drv_byte_fifo_read(drv_byte_fifo_t *fifo, uint8_t *dst,
                               uint16_t capacity, uint16_t *read_size)
{
    uint16_t n;
    uint16_t i;

    if ((fifo == NULL) || (dst == NULL) || (read_size == NULL) ||
        (capacity == 0U)) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    *read_size = 0U;
    if (!fifo->initialized || (fifo->storage == NULL) ||
        (fifo->capacity == 0U)) {
        return BS_STATUS_NOT_INITIALIZED;
    }
    if (fifo->used == 0U) {
        return BS_STATUS_EMPTY;
    }

    n = (capacity < fifo->used) ? capacity : fifo->used;
    for (i = 0U; i < n; ++i) {
        dst[i] = fifo->storage[fifo->head];
        ++fifo->head;
        if (fifo->head == fifo->capacity) {
            fifo->head = 0U;
        }
    }
    fifo->used = (uint16_t)(fifo->used - n);
    *read_size = n;
    return BS_STATUS_OK;
}

bs_status_t drv_byte_fifo_get_stats(const drv_byte_fifo_t *fifo,
                                    drv_byte_fifo_stats_t *stats)
{
    if ((fifo == NULL) || (stats == NULL)) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!fifo->initialized || (fifo->storage == NULL) ||
        (fifo->capacity == 0U)) {
        return BS_STATUS_NOT_INITIALIZED;
    }

    stats->capacity = fifo->capacity;
    stats->pending_bytes = fifo->used;
    stats->dropped_bytes = fifo->dropped_bytes;
    return BS_STATUS_OK;
}

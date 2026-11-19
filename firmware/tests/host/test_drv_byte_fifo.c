#include "drv_byte_fifo.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

int main(void)
{
    uint8_t storage[4] = {0U};
    uint8_t input1[] = {1U, 2U, 3U, 4U};
    uint8_t input2[] = {5U, 6U, 7U};
    uint8_t output[5] = {0U};
    uint16_t taken = 0xFFFFU;
    drv_byte_fifo_t fifo = {0};
    drv_byte_fifo_stats_t stats = {0};

    assert(drv_byte_fifo_write_all(&fifo, input1, 1U)
           == BS_STATUS_NOT_INITIALIZED);
    assert(drv_byte_fifo_init(NULL, storage, 4U)
           == BS_STATUS_INVALID_ARGUMENT);
    assert(drv_byte_fifo_init(&fifo, storage, 0U)
           == BS_STATUS_INVALID_ARGUMENT);
    assert(drv_byte_fifo_init(&fifo, storage, 4U) == BS_STATUS_OK);
    assert(drv_byte_fifo_read(&fifo, output, 5U, &taken) == BS_STATUS_EMPTY);
    assert(taken == 0U);
    assert(drv_byte_fifo_write_all(&fifo, input1, 4U) == BS_STATUS_OK);
    assert(drv_byte_fifo_write_all(&fifo, input2, 3U)
           == BS_STATUS_NO_RESOURCES);
    assert(drv_byte_fifo_get_stats(&fifo, &stats) == BS_STATUS_OK);
    assert(stats.pending_bytes == 4U && stats.dropped_bytes == 3U);

    assert(drv_byte_fifo_read(&fifo, output, 3U, &taken) == BS_STATUS_OK);
    assert(taken == 3U && output[0] == 1U && output[2] == 3U);
    assert(drv_byte_fifo_write_all(&fifo, input2, 3U) == BS_STATUS_OK);
    input2[0] = 99U; /* 写入时已经复制，证明不依赖源缓存生命周期。 */
    assert(drv_byte_fifo_read(&fifo, output, 5U, &taken) == BS_STATUS_OK);
    assert(taken == 4U);
    assert(output[0] == 4U && output[1] == 5U &&
           output[2] == 6U && output[3] == 7U);
    assert(drv_byte_fifo_read(&fifo, output, 5U, &taken) == BS_STATUS_EMPTY);
    assert(taken == 0U);
    assert(drv_byte_fifo_write_all(&fifo, NULL, 1U)
           == BS_STATUS_INVALID_ARGUMENT);
    assert(drv_byte_fifo_read(&fifo, NULL, 1U, &taken)
           == BS_STATUS_INVALID_ARGUMENT);
    assert(drv_byte_fifo_get_stats(&fifo, &stats) == BS_STATUS_OK);
    assert(stats.pending_bytes == 0U && stats.dropped_bytes == 3U);
    assert(drv_byte_fifo_init(&fifo, storage, 4U) == BS_STATUS_OK);
    assert(drv_byte_fifo_get_stats(&fifo, &stats) == BS_STATUS_OK);
    assert(stats.dropped_bytes == 0U);
    return 0;
}

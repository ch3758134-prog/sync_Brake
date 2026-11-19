#ifndef DRV_BYTE_FIFO_H
#define DRV_BYTE_FIFO_H

#include <stdbool.h>
#include <stdint.h>
#include "bs_status.h"

/* 固定容量字节队列；调用方持有并保证 storage 生命周期覆盖 FIFO 生命周期。
 * 只允许单执行上下文调用。ISR/主循环并发时，外层 Port 必须完整互斥。
 */
typedef struct {
    uint8_t *storage;
    uint16_t capacity;
    uint16_t head;
    uint16_t tail;
    uint16_t used;
    uint32_t dropped_bytes;
    bool initialized;
} drv_byte_fifo_t;

typedef struct {
    uint16_t capacity;
    uint16_t pending_bytes;
    uint32_t dropped_bytes;
} drv_byte_fifo_stats_t;

/* 初始化会清空原有内容；使用前需先停止该 FIFO 的并发访问。 */
bs_status_t drv_byte_fifo_init(drv_byte_fifo_t *fifo,
                               uint8_t *storage, uint16_t capacity);

/* 全部复制或全部拒绝。NO_RESOURCES 表示本次没有写入任何字节；
 * 被拒绝字节数计入 dropped_bytes（饱和计数），禁止静默截断。
 */
bs_status_t drv_byte_fifo_write_all(drv_byte_fifo_t *fifo,
                                    const uint8_t *src, uint16_t size);

/* 最多复制 capacity 字节。空 FIFO 返回 EMPTY，*read_size = 0。
 * 返回 OK 时 *read_size > 0；不会阻塞等待更多数据。
 */
bs_status_t drv_byte_fifo_read(drv_byte_fifo_t *fifo, uint8_t *dst,
                               uint16_t capacity, uint16_t *read_size);

bs_status_t drv_byte_fifo_get_stats(const drv_byte_fifo_t *fifo,
                                    drv_byte_fifo_stats_t *stats);

#endif

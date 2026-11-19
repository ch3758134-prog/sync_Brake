#ifndef DRV_UART_H
#define DRV_UART_H
#include "drv_types.h"
typedef enum {
    DRV_UART_EVENT_TX_WIRE_COMPLETE = 0,
    DRV_UART_EVENT_TX_FAILED,
    DRV_UART_EVENT_RX_OVERFLOW,
    DRV_UART_EVENT_LINE_ERROR
} drv_uart_event_kind_t;
typedef struct {
    drv_uart_event_kind_t kind;
    uint32_t request_id; /* 仅 TX 相关事件有效 */
    uint32_t observed_at_ms;
    uint32_t detail; /* 底层已归一化的错误/丢弃数量，编码待审 */
} drv_uart_event_t;
typedef struct {
    uint16_t tx_max_request_bytes;  /* 内部静态复制缓冲上限 */
    uint16_t rx_buffer_bytes;
    bool wire_completion_observable;
} drv_uart_capabilities_t;
typedef struct {
    uint32_t received_bytes;
    uint32_t dropped_bytes;
    uint32_t tx_completed;
    uint32_t tx_failed;
    bool rx_active;
    bool tx_busy;
} drv_uart_status_t;
/* 端口实际波特率、帧格式等由 Board/Variant 静态配置决定。 */
bs_status_t drv_uart_init(drv_uart_id_t id);
bs_status_t drv_uart_get_capabilities(drv_uart_id_t id,
                                      drv_uart_capabilities_t *caps);
/* 整笔复制到内部有界缓冲后返回 OK，调用方可立即复用 data。
 * OK ≠ 已发出；禁止无声截断；队列/并发限制由 caps 和 BUSY 报告。 */
bs_status_t drv_uart_send(drv_uart_id_t id, const uint8_t *data,
                          uint16_t size, uint32_t *request_id);
/* 非阻塞复制当前已有的字节，EMPTY 表示尚无数据，绝不等待指定长度。 */
bs_status_t drv_uart_read(drv_uart_id_t id, uint8_t *dst,
                          uint16_t capacity, uint16_t *read_size);
bs_status_t drv_uart_take_event(drv_uart_id_t id, drv_uart_event_t *event);
bs_status_t drv_uart_get_status(drv_uart_id_t id, drv_uart_status_t *status);
/* abort 必须报告已有发送是否可能已部分离开发送引脚。 */
bs_status_t drv_uart_abort_tx(drv_uart_id_t id, uint32_t request_id);
#endif

#ifndef DRV_UART_PORT_H
#define DRV_UART_PORT_H
#include "drv_uart.h"
/* BSW 内部接口：仅由公共 UART Core 与芯片适配实现使用。 */
bs_status_t drv_uart_port_init(drv_uart_id_t id);
bs_status_t drv_uart_port_start_tx(drv_uart_id_t id,
                                   const uint8_t *data, uint16_t size,
                                   uint32_t request_id);
bs_status_t drv_uart_port_cancel_tx(drv_uart_id_t id);
/* IRQ/DMA 回调仅交付有界字节或状态，禁止解析具体 BLE 协议。 */
void drv_uart_core_on_rx_isr(drv_uart_id_t id, const uint8_t *data,
                             uint16_t size, uint32_t observed_at_ms);
typedef enum { DRV_UART_PORT_DMA_TRANSFER_DONE = 0,
               DRV_UART_PORT_WIRE_COMPLETE,
               DRV_UART_PORT_TX_ERROR } drv_uart_port_tx_event_t;
void drv_uart_core_on_tx_isr(drv_uart_id_t id, uint32_t request_id,
                             drv_uart_port_tx_event_t kind,
                             uint32_t observed_at_ms);
void drv_uart_core_on_error_isr(drv_uart_id_t id, uint32_t flags,
                                uint32_t observed_at_ms);
#endif

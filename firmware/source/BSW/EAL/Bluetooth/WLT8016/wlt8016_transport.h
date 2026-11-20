#ifndef BRAKESYNC_WLT8016_TRANSPORT_H
#define BRAKESYNC_WLT8016_TRANSPORT_H
#include <stdbool.h>
#include <stdint.h>
#include "mcal_uart.h"
/* WLT8016 UART 透传链路；没有模块连接状态脚时，不能据此判断 BLE 已连接。 */
/* 仅初始化 MCU 侧串口；真实模块的波特率、TX/RX 接线必须核对。 */
bool WLT8016_TransportInit(void);
/* 非阻塞：一次发送长度最大 192 字节，发送完成需 MCAL UART 后续 Poll。 */
mcal_uart_result_t WLT8016_TransportWrite(const uint8_t *data, uint16_t size);
uint16_t WLT8016_TransportRead(uint8_t *dst, uint16_t capacity);
void WLT8016_TransportPoll(void);
#endif
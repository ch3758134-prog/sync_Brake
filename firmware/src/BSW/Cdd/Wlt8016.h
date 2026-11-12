#ifndef BRAKESYNC_WLT8016_H
#define BRAKESYNC_WLT8016_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "bsw_port.h"
/* BLE 模块的 UART 字节搬运层；绝不猜测固件 AT 指令 / GATT UUID。 */
typedef enum { WLT8016_DISCONNECTED, WLT8016_CONNECTED, WLT8016_UNKNOWN } Wlt8016Connection;
typedef struct { uint8_t uart_index; Wlt8016Connection connection; bool initialized; } Wlt8016;
BsStatus Wlt8016_Init(Wlt8016 *dev, uint8_t uart_index);
void Wlt8016_ReportConnection(Wlt8016 *dev, Wlt8016Connection state);
BsStatus Wlt8016_Receive(Wlt8016 *dev, uint8_t *data, size_t size, size_t *received);
BsStatus Wlt8016_Send(Wlt8016 *dev, const uint8_t *data, size_t size, uint32_t timeout_ms);
#endif

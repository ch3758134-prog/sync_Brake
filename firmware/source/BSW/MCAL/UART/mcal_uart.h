#ifndef BRAKESYNC_MCAL_UART_H
#define BRAKESYNC_MCAL_UART_H
#include <stdbool.h>
#include <stdint.h>

/* 仅当前 YTM32B1LE05 EVB 的 UART1，PTC8 RX/PTC9 TX、YCT 115200 8N1。 */
typedef enum {
    MCAL_UART_OK = 0,
    MCAL_UART_BUSY,
    MCAL_UART_INVALID_ARGUMENT,
    MCAL_UART_NOT_READY,
    MCAL_UART_HARDWARE_ERROR
} mcal_uart_result_t;
typedef struct {
    uint32_t rx_received;
    uint32_t rx_dropped;
    uint32_t rx_errors;
    uint32_t tx_completed;
    uint32_t tx_errors;
    bool tx_busy;
} mcal_uart_stats_t;

/* 配置时钟和 PinMux 已由 system_init() 完成；本层启动云途 UART IRQ 驱动。 */
mcal_uart_result_t MCAL_UART1_Init(void);
/* 不阻塞。复制整笔数据到驱动内部有界缓冲，返回 OK 不代表已经发完。 */
mcal_uart_result_t MCAL_UART1_Write(const uint8_t *data, uint16_t size);
/* 无数据时返回 0。读取的是中断接收环形缓冲，不解析 BLE 协议。 */
uint16_t MCAL_UART1_Read(uint8_t *dst, uint16_t capacity);
/* 主循环周期调用，更新 TX 完成/错误状态，恢复异常终止的 RX。 */
void MCAL_UART1_Poll(void);
void MCAL_UART1_GetStats(mcal_uart_stats_t *out);
#endif
#ifndef BRAKESYNC_TARGET_TEST_UART1_H
#define BRAKESYNC_TARGET_TEST_UART1_H
#include <stdbool.h>
#include <stdint.h>
/* Keil Watch 观测；只是 EVB UART1 诊断，不会主动向 BLE 发送控制命令。 */
extern volatile uint32_t g_uart1_core_clock_hz;
extern volatile uint32_t g_uart1_source_clock_hz;
extern volatile uint32_t g_uart1_received_bytes;
extern volatile uint32_t g_uart1_dropped_bytes;
extern volatile uint32_t g_uart1_last_byte;
bool test_uart1_init(void);
void test_uart1_run_once(void);
#endif
#include "mcal_uart.h"
#include "uart_driver.h"
#include "uart_config.h"
#include "device_registers.h"
#include <stddef.h>
#include <string.h>

#define UART_INSTANCE     (1U)
#define UART_RX_CAPACITY  (256U)
#define UART_TX_CAPACITY  (192U)

static volatile uint8_t s_rx[UART_RX_CAPACITY];
static volatile uint16_t s_rx_head;
static volatile uint16_t s_rx_tail;
static uint8_t s_rx_byte;
static uint8_t s_tx[UART_TX_CAPACITY];
static volatile uint32_t s_rx_received;
static volatile uint32_t s_rx_dropped;
static volatile uint32_t s_rx_errors;
static uint32_t s_tx_completed;
static uint32_t s_tx_errors;
static bool s_ready;
static bool s_tx_busy;
static volatile bool s_rx_restart_needed;

/* 该回调在厂商 UART1 IRQ 上下文内调用。SDK 已负责 RX FIFO 取数。 */
static void OnRx(void *driver_state, uart_event_t event, void *user_data)
{
    uint16_t next;
    (void)driver_state;
    (void)user_data;
    if (event == UART_EVENT_RX_FULL) {
        next = (uint16_t)((s_rx_head + 1U) % UART_RX_CAPACITY);
        if (next != s_rx_tail) {
            s_rx[s_rx_head] = s_rx_byte;
            s_rx_head = next;
            ++s_rx_received;
        } else {
            ++s_rx_dropped;
        }
        /* 重要：在 RX_FULL 回调中续设 1B 缓冲，避免每字节停启 UART。 */
        (void)UART_DRV_SetRxBuffer(UART_INSTANCE, &s_rx_byte, 1U);
    } else if (event == UART_EVENT_ERROR) {
        ++s_rx_errors;
        s_rx_restart_needed = true;
    }
}

mcal_uart_result_t MCAL_UART1_Init(void)
{
    status_t rc;
    if (s_ready) {
        return MCAL_UART_OK;
    }
    /* 防止 YCT 配置意外改成 DMA，却缺失真实 DMA request/channel。 */
    if (uart_config0.transferType != UART_USING_INTERRUPTS ||
        uart_config0.baudRate == 0U) {
        return MCAL_UART_INVALID_ARGUMENT;
    }
    s_rx_head = 0U;
    s_rx_tail = 0U;
    s_rx_received = 0U;
    s_rx_dropped = 0U;
    s_rx_errors = 0U;
    s_tx_completed = 0U;
    s_tx_errors = 0U;
    s_rx_restart_needed = false;
    rc = UART_DRV_Init(UART_INSTANCE, &uart_config0_State, &uart_config0);
    if (rc != STATUS_SUCCESS) {
        return MCAL_UART_HARDWARE_ERROR;
    }
    (void)UART_DRV_InstallRxCallback(UART_INSTANCE, OnRx, NULL);
    rc = UART_DRV_ReceiveData(UART_INSTANCE, &s_rx_byte, 1U);
    if (rc != STATUS_SUCCESS) {
        (void)UART_DRV_Deinit(UART_INSTANCE);
        return MCAL_UART_HARDWARE_ERROR;
    }
    s_ready = true;
    return MCAL_UART_OK;
}

mcal_uart_result_t MCAL_UART1_Write(const uint8_t *data, uint16_t size)
{
    status_t rc;
    if (!s_ready) { return MCAL_UART_NOT_READY; }
    if (data == NULL || size == 0U || size > UART_TX_CAPACITY) {
        return MCAL_UART_INVALID_ARGUMENT;
    }
    if (s_tx_busy) { return MCAL_UART_BUSY; }
    memcpy(s_tx, data, size);
    rc = UART_DRV_SendData(UART_INSTANCE, s_tx, size);
    if (rc == STATUS_BUSY) { return MCAL_UART_BUSY; }
    if (rc != STATUS_SUCCESS) { return MCAL_UART_HARDWARE_ERROR; }
    s_tx_busy = true;
    return MCAL_UART_OK;
}

uint16_t MCAL_UART1_Read(uint8_t *dst, uint16_t capacity)
{
    uint16_t count = 0U;
    uint32_t old_mask;
    if (!s_ready || dst == NULL || capacity == 0U) { return 0U; }
    /* 简短临界区，只复制现有字节；保留原 PRIMASK。 */
    old_mask = __get_PRIMASK();
    __disable_irq();
    while (count < capacity && s_rx_tail != s_rx_head) {
        dst[count++] = s_rx[s_rx_tail];
        s_rx_tail = (uint16_t)((s_rx_tail + 1U) % UART_RX_CAPACITY);
    }
    __set_PRIMASK(old_mask);
    return count;
}

void MCAL_UART1_Poll(void)
{
    status_t rc;
    if (!s_ready) { return; }
    if (s_tx_busy) {
        rc = UART_DRV_GetTransmitStatus(UART_INSTANCE, NULL);
        if (rc != STATUS_BUSY) {
            s_tx_busy = false;
            if (rc == STATUS_SUCCESS) { ++s_tx_completed; }
            else { ++s_tx_errors; }
        }
    }
    if (s_rx_restart_needed) {
        /* 错误中断可能已结束硬件接收；先在非 ISR 上下文进行恢复。 */
        rc = UART_DRV_GetReceiveStatus(UART_INSTANCE, NULL);
        if (rc != STATUS_BUSY) {
            s_rx_restart_needed = false;
            if (UART_DRV_ReceiveData(UART_INSTANCE, &s_rx_byte, 1U) != STATUS_SUCCESS) {
                ++s_rx_errors;
                s_rx_restart_needed = true;
            }
        }
    }
}

void MCAL_UART1_GetStats(mcal_uart_stats_t *out)
{
    uint32_t old_mask;
    if (out == NULL) { return; }
    old_mask = __get_PRIMASK();
    __disable_irq();
    out->rx_received = s_rx_received;
    out->rx_dropped = s_rx_dropped;
    out->rx_errors = s_rx_errors;
    out->tx_completed = s_tx_completed;
    out->tx_errors = s_tx_errors;
    out->tx_busy = s_tx_busy;
    __set_PRIMASK(old_mask);
}
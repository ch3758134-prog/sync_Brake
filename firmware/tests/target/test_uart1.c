#include "test_uart1.h"
#include "mcal_clock.h"
#include "wlt8016_transport.h"

volatile uint32_t g_uart1_core_clock_hz;
volatile uint32_t g_uart1_source_clock_hz;
volatile uint32_t g_uart1_received_bytes;
volatile uint32_t g_uart1_dropped_bytes;
volatile uint32_t g_uart1_last_byte;

bool test_uart1_init(void)
{
    g_uart1_core_clock_hz = MCAL_CLOCK_GetFrequency(MCAL_CLOCK_CORE);
    g_uart1_source_clock_hz = MCAL_CLOCK_GetFrequency(MCAL_CLOCK_UART1);
    g_uart1_received_bytes = 0U;
    g_uart1_dropped_bytes = 0U;
    g_uart1_last_byte = 0U;
    if (g_uart1_source_clock_hz == 0U) {
        return false;
    }
    return WLT8016_TransportInit();
}

void test_uart1_run_once(void)
{
    uint8_t buffer[32];
    uint16_t i;
    uint16_t n;
    mcal_uart_stats_t stats;
    WLT8016_TransportPoll();
    n = WLT8016_TransportRead(buffer, (uint16_t)sizeof(buffer));
    for (i = 0U; i < n; ++i) {
        g_uart1_last_byte = buffer[i];
    }
    MCAL_UART1_GetStats(&stats);
    g_uart1_received_bytes = stats.rx_received;
    g_uart1_dropped_bytes = stats.rx_dropped;
}
#include "Wlt8016.h"
BsStatus Wlt8016_Init(Wlt8016 *dev, uint8_t uart_index)
{
    if (!dev) return BS_E_ARGUMENT;
    dev->uart_index = uart_index;
    dev->connection = WLT8016_UNKNOWN;
    dev->initialized = true;
    return BS_OK;
}
void Wlt8016_ReportConnection(Wlt8016 *dev, Wlt8016Connection state)
{
    if (dev) dev->connection = state;
}
BsStatus Wlt8016_Receive(Wlt8016 *dev, uint8_t *data, size_t size, size_t *received)
{
    if (!dev || !dev->initialized) return BS_E_ARGUMENT;
    return Bs_UartRead(dev->uart_index, data, size, received);
}
BsStatus Wlt8016_Send(Wlt8016 *dev, const uint8_t *data, size_t size, uint32_t timeout_ms)
{
    if (!dev || !dev->initialized) return BS_E_ARGUMENT;
    if (dev->connection != WLT8016_CONNECTED) return BS_E_UNBOUND;
    return Bs_UartWrite(dev->uart_index, data, size, timeout_ms);
}

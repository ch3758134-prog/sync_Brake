#include "wlt8016_transport.h"
/* WLT8016 透明 UART byte stream 层：不解析 AT、GATT 或产品应用报文。 */
bool WLT8016_TransportInit(void)
{
    return MCAL_UART1_Init() == MCAL_UART_OK;
}
mcal_uart_result_t WLT8016_TransportWrite(const uint8_t *data, uint16_t size)
{
    return MCAL_UART1_Write(data, size);
}
uint16_t WLT8016_TransportRead(uint8_t *dst, uint16_t capacity)
{
    return MCAL_UART1_Read(dst, capacity);
}
void WLT8016_TransportPoll(void)
{
    MCAL_UART1_Poll();
}
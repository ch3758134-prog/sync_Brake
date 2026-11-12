#include "bsw_port.h"
extern const BsPort *Bs_PortGet(void);
BsStatus Bs_UartRead(uint8_t bus, uint8_t *out, size_t capacity, size_t *received)
{
    const BsPort *p = Bs_PortGet();
    if (!out || !received || !capacity) return BS_E_ARGUMENT;
    *received = 0u;
    if (!p || !p->uart_read) return BS_E_UNBOUND;
    return p->uart_read((p->uart_context ? p->uart_context : p->context), bus, out, capacity, received);
}
BsStatus Bs_UartWrite(uint8_t bus, const uint8_t *data, size_t count, uint32_t timeout_ms)
{
    const BsPort *p = Bs_PortGet();
    if (!data || !count || timeout_ms == 0u) return BS_E_ARGUMENT;
    if (!p || !p->uart_write) return BS_E_UNBOUND;
    return p->uart_write((p->uart_context ? p->uart_context : p->context), bus, data, count, timeout_ms);
}

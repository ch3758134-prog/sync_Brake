#include "bsw_port.h"
extern const BsPort *Bs_PortGet(void);
BsStatus Bs_TimerNowMs(uint32_t *now)
{
    const BsPort *p = Bs_PortGet();
    if (!now) return BS_E_ARGUMENT;
    if (!p || !p->timer_now_ms) return BS_E_UNBOUND;
    return p->timer_now_ms((p->timer_context ? p->timer_context : p->context), now);
}

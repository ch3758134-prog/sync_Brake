#include "bsw_port.h"
extern const BsPort *Bs_PortGet(void);
static BsStatus Validate(const BsCanFrame *f)
{
    if (!f) return BS_E_ARGUMENT;
    if (f->dlc > 8u) return BS_E_RANGE;
    if (f->extended ? (f->id > 0x1FFFFFFFu) : (f->id > 0x7FFu)) return BS_E_RANGE;
    return BS_OK;
}
BsStatus Bs_CanSend(uint8_t bus, const BsCanFrame *frame)
{
    const BsPort *p = Bs_PortGet();
    BsStatus s = Validate(frame);
    if (s != BS_OK) return s;
    if (!p || !p->can_send) return BS_E_UNBOUND;
    return p->can_send(p->context, bus, frame);
}
BsStatus Bs_CanReceive(uint8_t bus, BsCanFrame *frame)
{
    const BsPort *p = Bs_PortGet();
    BsStatus s;
    if (!frame) return BS_E_ARGUMENT;
    if (!p || !p->can_receive) return BS_E_UNBOUND;
    s = p->can_receive(p->context, bus, frame);
    return s == BS_OK ? Validate(frame) : s;
}

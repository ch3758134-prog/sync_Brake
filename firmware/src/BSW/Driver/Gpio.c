#include "bsw_port.h"
/* GPIO 具体 pinmux、上下拉和复用由板级 SDK 初始化并记录；此处只做 IO 调用。 */
extern const BsPort *Bs_PortGet(void);
BsStatus Bs_GpioRead(uint16_t pin, bool *level)
{
    const BsPort *p = Bs_PortGet();
    if (!level) return BS_E_ARGUMENT;
    if (!p || !p->gpio_read) return BS_E_UNBOUND;
    return p->gpio_read((p->gpio_context ? p->gpio_context : p->context), pin, level);
}
BsStatus Bs_GpioWrite(uint16_t pin, bool level)
{
    const BsPort *p = Bs_PortGet();
    if (!p || !p->gpio_write) return BS_E_UNBOUND;
    return p->gpio_write((p->gpio_context ? p->gpio_context : p->context), pin, level);
}

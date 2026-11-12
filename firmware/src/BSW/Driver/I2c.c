#include "bsw_port.h"
extern const BsPort *Bs_PortGet(void);
BsStatus Bs_I2cRegRead(uint8_t bus, uint8_t addr7, uint8_t reg, uint8_t *out, size_t count)
{
    const BsPort *p = Bs_PortGet();
    if (!out || !count || addr7 > 0x7fu) return BS_E_ARGUMENT;
    if (!p || !p->i2c_reg_read) return BS_E_UNBOUND;
    return p->i2c_reg_read((p->i2c_context ? p->i2c_context : p->context), bus, addr7, reg, out, count);
}
BsStatus Bs_I2cRegWrite(uint8_t bus, uint8_t addr7, uint8_t reg, const uint8_t *data, size_t count)
{
    const BsPort *p = Bs_PortGet();
    if (!data || !count || addr7 > 0x7fu) return BS_E_ARGUMENT;
    if (!p || !p->i2c_reg_write) return BS_E_UNBOUND;
    return p->i2c_reg_write((p->i2c_context ? p->i2c_context : p->context), bus, addr7, reg, data, count);
}

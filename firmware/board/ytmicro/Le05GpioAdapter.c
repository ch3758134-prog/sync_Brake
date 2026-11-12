#include "Le05GpioAdapter.h"
#include <stddef.h>

/* 不隐式执行引脚复用/方向配置：由已经核对的 Board PinMux 负责。 */
static const Le05GpioPin *FindPin(const Le05GpioMapping *mapping, uint16_t id)
{
    size_t i;
    if (!mapping || !mapping->pins) return NULL;
    for (i = 0u; i < mapping->count; ++i) {
        if (mapping->pins[i].logical_id == id) return &mapping->pins[i];
    }
    return NULL;
}
static BsStatus ReadGpio(void *context, uint16_t id, bool *level)
{
    const Le05GpioPin *pin = FindPin((Le05GpioMapping *)context, id);
    if (!level) return BS_E_ARGUMENT;
    if (!pin || !pin->base || pin->bit_index >= 32u) return BS_E_RANGE;
    *level = PINS_DRV_ReadPin(pin->base, pin->bit_index) != 0u;
    return BS_OK;
}
static BsStatus WriteGpio(void *context, uint16_t id, bool level)
{
    const Le05GpioPin *pin = FindPin((Le05GpioMapping *)context, id);
    if (!pin || !pin->base || pin->bit_index >= 32u) return BS_E_RANGE;
    PINS_DRV_WritePin(pin->base, pin->bit_index, level ? 1u : 0u);
    return BS_OK;
}
BsStatus Le05Gpio_Install(BsPort *port, Le05GpioMapping *mapping)
{
    size_t i, j;
    if (!port || !mapping || !mapping->pins || mapping->count == 0u)
        return BS_E_ARGUMENT;
    for (i=0u; i<mapping->count; ++i) {
        if (!mapping->pins[i].base || mapping->pins[i].bit_index >= 32u)
            return BS_E_RANGE;
        for (j=i+1u; j<mapping->count; ++j) {
            if (mapping->pins[i].logical_id == mapping->pins[j].logical_id)
                return BS_E_ARGUMENT;
        }
    }
    /* 不占用其他外设 context，避免 GPIO 与 CAN/UART/I2C 绑定互相覆盖。 */
    if (port->gpio_context || port->gpio_read || port->gpio_write)
        return BS_E_BUSY;
    port->gpio_context = mapping;
    port->gpio_read = ReadGpio;
    port->gpio_write = WriteGpio;
    return BS_OK;
}

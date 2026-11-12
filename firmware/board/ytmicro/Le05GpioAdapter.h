#ifndef BRAKESYNC_LE05_GPIO_ADAPTER_H
#define BRAKESYNC_LE05_GPIO_ADAPTER_H

#include "bsw_port.h"
/* 仅用于真实 YTM32B1LE0 SDK 1.5.0 Target 编译，不纳入 Host Build。 */
#include "pins_driver.h"

typedef struct {
    uint16_t logical_id; /* 来自独立 Board 配置，不是 MCU 端口号 */
    GPIO_Type *base;
    uint8_t bit_index;   /* 0..31，必须是 GPIO 位编号而非位掩码 */
} Le05GpioPin;

typedef struct {
    const Le05GpioPin *pins;
    size_t count;
} Le05GpioMapping;

/* 填充 BsPort 的 GPIO 部分，不修改其他 UART/CAN/I2C/Timer 绑定。 */
BsStatus Le05Gpio_Install(BsPort *port, Le05GpioMapping *mapping);
#endif

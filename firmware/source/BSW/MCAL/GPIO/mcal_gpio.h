#ifndef BRAKESYNC_MCAL_GPIO_H
#define BRAKESYNC_MCAL_GPIO_H

#include <stdbool.h>
#include <stdint.h>

/* 当前 YTM32B1LE0 MCU 的 GPIO 端口。MCAL 允许与具体芯片相关。 */
typedef enum {
    MCAL_GPIO_PORT_A = 0,
    MCAL_GPIO_PORT_B,
    MCAL_GPIO_PORT_C,
    MCAL_GPIO_PORT_D,
    MCAL_GPIO_PORT_E,
    MCAL_GPIO_PORT_COUNT
} mcal_gpio_port_t;

typedef enum {
    MCAL_GPIO_INPUT = 0,
    MCAL_GPIO_OUTPUT
} mcal_gpio_direction_t;

typedef enum {
    MCAL_GPIO_PULL_NONE = 0,
    MCAL_GPIO_PULL_DOWN,
    MCAL_GPIO_PULL_UP
} mcal_gpio_pull_t;

typedef enum {
    MCAL_GPIO_DRIVE_LOW = 0,
    MCAL_GPIO_DRIVE_HIGH
} mcal_gpio_drive_t;

typedef enum {
    MCAL_GPIO_SLEW_FAST = 0,
    MCAL_GPIO_SLEW_SLOW
} mcal_gpio_slew_t;

typedef enum {
    MCAL_GPIO_IRQ_DISABLED = 0,
    MCAL_GPIO_IRQ_RISING,
    MCAL_GPIO_IRQ_FALLING,
    MCAL_GPIO_IRQ_BOTH_EDGES
} mcal_gpio_irq_mode_t;

typedef struct {
    mcal_gpio_direction_t direction;
    uint8_t initial_level;           /* 仅对输出有效；MCU 原始电平 */
    mcal_gpio_pull_t pull;
    mcal_gpio_drive_t drive;
    mcal_gpio_slew_t slew;
    bool passive_filter;
    bool digital_filter;
    uint8_t digital_filter_width;   /* 具体时间单位由芯片/SDK 定义 */
} mcal_gpio_config_t;

typedef struct {
    uint32_t count;
    bool overflowed;
} mcal_gpio_events_t;

/*
 * 初始化单个 GPIO：复用、上下拉、驱动、转换速率、滤波、方向及初始电平。
 * 前提：MCU 时钟已初始化；同一引脚不再由 YCT 通用入口重复初始化。
 * 此函数只进行 GPIO 初始化，中断先保持禁止；返回值用于配置有效性，
 * 并不代表实际引脚电压或负载动作已经得到反馈。
 */
bool MCAL_GPIO_Init(mcal_gpio_port_t port,
                    uint8_t pin,
                    const mcal_gpio_config_t *config);

void MCAL_GPIO_Write(mcal_gpio_port_t port, uint8_t pin, uint8_t level);
uint8_t MCAL_GPIO_Read(mcal_gpio_port_t port, uint8_t pin);
void MCAL_GPIO_Toggle(mcal_gpio_port_t port, uint8_t pin);

/* 中断由 MCAL 统一管理，当前最多登记 8 个引脚。 */
bool MCAL_GPIO_ConfigureInterrupt(mcal_gpio_port_t port,
                                  uint8_t pin,
                                  mcal_gpio_irq_mode_t mode);
void MCAL_GPIO_EnableInterruptController(void);
mcal_gpio_events_t MCAL_GPIO_TakeEvents(mcal_gpio_port_t port, uint8_t pin);

#endif /* BRAKESYNC_MCAL_GPIO_H */
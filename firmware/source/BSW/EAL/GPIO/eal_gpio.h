#ifndef BRAKESYNC_EAL_GPIO_H
#define BRAKESYNC_EAL_GPIO_H

#include <stdbool.h>
#include <stdint.h>

/* 当前 YTM32B1LE05 EVB 板级信号，不是最终制动产品的 PCB 信号。 */
typedef enum
{
    EAL_GPIO_LED_HEARTBEAT = 0,
    EAL_GPIO_LED_BUTTON,
    EAL_GPIO_LED_COUNT
} eal_gpio_led_t;

typedef enum
{
    EAL_GPIO_BUTTON_1 = 0,
    EAL_GPIO_BUTTON_2,
    EAL_GPIO_BUTTON_COUNT
} eal_gpio_button_t;

typedef struct
{
    uint32_t count;
    bool overflowed;
} eal_gpio_events_t;

/* system_init() 后调用；false 表示中断资源配置失败。 */
bool EAL_GPIO_Init(void);

/* raw_level 是芯片 GPIO 逻辑电平，LED 亮灭极性仍需实测。 */
void EAL_GPIO_WriteLed(eal_gpio_led_t led, uint8_t raw_level);
void EAL_GPIO_ToggleLed(eal_gpio_led_t led);
uint8_t EAL_GPIO_ReadButton(eal_gpio_button_t button);
eal_gpio_events_t EAL_GPIO_TakeButtonEvents(eal_gpio_button_t button);

#endif /* BRAKESYNC_EAL_GPIO_H */
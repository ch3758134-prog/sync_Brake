#ifndef GPIO_BRINGUP_H
#define GPIO_BRINGUP_H

#include <stdint.h>
#include "bs_status.h"

/* EVB_NO_BRAKE_POWER：只演示两颗 LED 与实体 GPIO 按键。 */
bs_status_t gpio_bringup_init(uint32_t now_ms);
bs_status_t gpio_bringup_poll(uint32_t now_ms);
uint32_t gpio_bringup_get_button_irq_count(void);

#endif

#ifndef BRAKESYNC_GPIO_BRINGUP_H
#define BRAKESYNC_GPIO_BRINGUP_H

#include <stdint.h>

/* 只用于当前开发板无功率测试，不属于产品制动业务。 */
void gpio_bringup_init(uint32_t now_ms);
void gpio_bringup_poll(uint32_t now_ms);
uint32_t gpio_bringup_get_button_irq_count(void);

#endif

#ifndef DRV_GPIO_H
#define DRV_GPIO_H
#include "drv_types.h"
typedef struct {
    uint32_t count;         /* 已累计的事件，可能被限制到上限 */
    bool overflowed;        /* 存在无法完整计数的事件 */
} drv_gpio_events_t;
/* BoardCfg 初始化方向、Pull、初始电平和 IO 复用。此 API 不能改变这些配置。 */
bs_status_t drv_gpio_read(drv_gpio_id_t id, bool *level);
bs_status_t drv_gpio_write(drv_gpio_id_t id, bool level);
/* 仅能使能已在 BoardCfg 配置好的 IRQ；不可修改引脚的电气配置。 */
bs_status_t drv_gpio_set_irq_enabled(drv_gpio_id_t id, bool enabled);
/* 原子地读取并清除事件；需验证 ISR/主循环竞态处理。 */
bs_status_t drv_gpio_take_events(drv_gpio_id_t id, drv_gpio_events_t *events);
#endif

#include "drv_gpio.h"
#include "board_gpio_ids.h"

#include "device_registers.h"
#include "interrupt_manager.h"
#include "pins_driver.h"

#include <stddef.h>
#include <stdint.h>

/* EVB GPIO 适配：公共 API 无 MCU 类型，映射与 IRQ 均局限在本文件。 */
typedef struct {
    drv_gpio_id_t id;
    GPIO_Type *gpio;
    uint32_t pin;
    bool input;
    bool output;
    bool interrupt;
} evb_gpio_binding_t;

static const evb_gpio_binding_t gpio_map[] = {
    {BOARD_GPIO_EVB_HEARTBEAT_LED, GPIOD, 7U, true, true, false},
    {BOARD_GPIO_EVB_BUTTON_LED, GPIOD, 6U, true, true, false},
    {BOARD_GPIO_EVB_BUTTON_1, GPIOE, 0U, true, false, true},
    {BOARD_GPIO_EVB_BUTTON_2, GPIOE, 1U, true, false, true}
};

typedef struct {
    volatile uint32_t count;
    volatile bool overflowed;
} evb_gpio_irq_slot_t;

static evb_gpio_irq_slot_t button_events[2];

static const evb_gpio_binding_t *find_gpio(drv_gpio_id_t id)
{
    uint32_t i;
    for (i = 0U; i < (sizeof(gpio_map) / sizeof(gpio_map[0])); ++i) {
        if (gpio_map[i].id == id) {
            return &gpio_map[i];
        }
    }
    return NULL;
}

bs_status_t drv_gpio_read(drv_gpio_id_t id, bool *level)
{
    const evb_gpio_binding_t *binding = find_gpio(id);
    if (level == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (binding == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!binding->input) {
        return BS_STATUS_NOT_AUTHORIZED;
    }
    *level = (PINS_DRV_ReadPin(binding->gpio, binding->pin) != 0U);
    return BS_STATUS_OK;
}

bs_status_t drv_gpio_write(drv_gpio_id_t id, bool level)
{
    const evb_gpio_binding_t *binding = find_gpio(id);
    if (binding == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!binding->output) {
        return BS_STATUS_NOT_AUTHORIZED;
    }
    PINS_DRV_WritePin(binding->gpio, binding->pin, level ? 1U : 0U);
    return BS_STATUS_OK;
}

bs_status_t drv_gpio_set_irq_enabled(drv_gpio_id_t id, bool enabled)
{
    const evb_gpio_binding_t *binding = find_gpio(id);
    uint32_t previous_mask;
    if (binding == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!binding->interrupt) {
        return BS_STATUS_UNSUPPORTED;
    }
    /* API 只允许在 BoardCfg 初始化后由主循环调用。 */
    previous_mask = __get_PRIMASK();
    __disable_irq();
    PINS_DRV_SetPinIntSel(binding->gpio, binding->pin,
                          enabled ? PCTRL_INT_RISING_EDGE : PCTRL_DMA_INT_DISABLED);
    PINS_DRV_ClearPinIntFlagCmd(binding->gpio, binding->pin);
    if (enabled) {
        INT_SYS_ClearPending(GPIO_IRQn);
        INT_SYS_EnableIRQ(GPIO_IRQn);
    }
    __set_PRIMASK(previous_mask);
    return BS_STATUS_OK;
}

bs_status_t drv_gpio_take_events(drv_gpio_id_t id, drv_gpio_events_t *events)
{
    uint32_t previous_mask;
    uint32_t index;
    if (events == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (id == BOARD_GPIO_EVB_BUTTON_1) {
        index = 0U;
    } else if (id == BOARD_GPIO_EVB_BUTTON_2) {
        index = 1U;
    } else {
        return (find_gpio(id) == NULL) ? BS_STATUS_INVALID_ARGUMENT
                                      : BS_STATUS_UNSUPPORTED;
    }
    /* 主循环读取并清空；保留调用前的 PRIMASK，不擅自打开中断。 */
    previous_mask = __get_PRIMASK();
    __disable_irq();
    events->count = button_events[index].count;
    events->overflowed = button_events[index].overflowed;
    button_events[index].count = 0U;
    button_events[index].overflowed = false;
    __set_PRIMASK(previous_mask);
    return BS_STATUS_OK;
}

static void record_button_irq(uint32_t index)
{
    if (button_events[index].count == UINT32_MAX) {
        button_events[index].overflowed = true;
    } else {
        ++button_events[index].count;
    }
}

/* 本 IRQ 由正式 Driver 唯一拥有，避免测试代码长期绑定硬件向量。 */
void GPIO_IRQHandler(void)
{
    const uint32_t flags = PINS_DRV_GetPortIntFlag(GPIOE);
    if ((flags & (1UL << 0U)) != 0U) {
        PINS_DRV_ClearPinIntFlagCmd(GPIOE, 0U);
        record_button_irq(0U);
    }
    if ((flags & (1UL << 1U)) != 0U) {
        PINS_DRV_ClearPinIntFlagCmd(GPIOE, 1U);
        record_button_irq(1U);
    }
}

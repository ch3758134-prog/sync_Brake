#include "gpio_bringup.h"

#include "eal_gpio.h"
#include "time_service.h"

#include <stdint.h>

#define HEARTBEAT_PERIOD_MS (500U)

static uint32_t s_last_heartbeat_ms;
static uint32_t s_button_irq_count;

static uint32_t SaturatingAdd(uint32_t left, uint32_t right)
{
    return (UINT32_MAX - left < right) ? UINT32_MAX : left + right;
}

bs_status_t gpio_bringup_init(uint32_t now_ms)
{
    s_last_heartbeat_ms = now_ms;
    s_button_irq_count = 0U;

    /* 通过正式 EAL 初始化，不再触碰 SDK 或旧 legacy 驱动。 */
    if (!EAL_GPIO_Init()) {
        return BS_STATUS_NO_RESOURCES;
    }
    return BS_STATUS_OK;
}

bs_status_t gpio_bringup_poll(uint32_t now_ms)
{
    eal_gpio_events_t first;
    eal_gpio_events_t second;

    if (time_service_has_elapsed(s_last_heartbeat_ms, HEARTBEAT_PERIOD_MS,
                                 now_ms)) {
        EAL_GPIO_ToggleLed(EAL_GPIO_LED_HEARTBEAT);
        s_last_heartbeat_ms = now_ms;
    }

    first = EAL_GPIO_TakeButtonEvents(EAL_GPIO_BUTTON_1);
    second = EAL_GPIO_TakeButtonEvents(EAL_GPIO_BUTTON_2);
    s_button_irq_count = SaturatingAdd(
        s_button_irq_count, SaturatingAdd(first.count, second.count));

    if ((first.count != 0U) || (second.count != 0U)) {
        EAL_GPIO_ToggleLed(EAL_GPIO_LED_BUTTON);
    }
    return (first.overflowed || second.overflowed) ?
           BS_STATUS_OVERFLOW : BS_STATUS_OK;
}

uint32_t gpio_bringup_get_button_irq_count(void)
{
    return s_button_irq_count;
}
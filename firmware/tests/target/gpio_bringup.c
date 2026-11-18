#include "gpio_bringup.h"

#include "board_gpio_ids.h"
#include "drv_gpio.h"
#include "time_service.h"

#include <stdint.h>

#define HEARTBEAT_PERIOD_MS (500U)

static uint32_t last_heartbeat_ms;
static uint32_t button_irq_count;

static bs_status_t toggle_led(drv_gpio_id_t id)
{
    bool current;
    bs_status_t status = drv_gpio_read(id, &current);
    if (status != BS_STATUS_OK) {
        return status;
    }
    return drv_gpio_write(id, !current);
}

static uint32_t saturating_add(uint32_t a, uint32_t b)
{
    return (UINT32_MAX - a < b) ? UINT32_MAX : a + b;
}

bs_status_t gpio_bringup_init(uint32_t now_ms)
{
    bs_status_t status;
    last_heartbeat_ms = now_ms;
    button_irq_count = 0U;
    status = drv_gpio_set_irq_enabled(BOARD_GPIO_EVB_BUTTON_1, true);
    if (status != BS_STATUS_OK) {
        return status;
    }
    status = drv_gpio_set_irq_enabled(BOARD_GPIO_EVB_BUTTON_2, true);
    if (status != BS_STATUS_OK) {
        (void)drv_gpio_set_irq_enabled(BOARD_GPIO_EVB_BUTTON_1, false);
    }
    return status;
}

bs_status_t gpio_bringup_poll(uint32_t now_ms)
{
    bs_status_t status;
    drv_gpio_events_t first = {0U, false};
    drv_gpio_events_t second = {0U, false};

    if (time_service_has_elapsed(last_heartbeat_ms, HEARTBEAT_PERIOD_MS, now_ms)) {
        status = toggle_led(BOARD_GPIO_EVB_HEARTBEAT_LED);
        if (status != BS_STATUS_OK) {
            return status;
        }
        last_heartbeat_ms = now_ms;
    }

    status = drv_gpio_take_events(BOARD_GPIO_EVB_BUTTON_1, &first);
    if (status != BS_STATUS_OK) {
        return status;
    }
    status = drv_gpio_take_events(BOARD_GPIO_EVB_BUTTON_2, &second);
    if (status != BS_STATUS_OK) {
        return status;
    }
    button_irq_count = saturating_add(
        button_irq_count, saturating_add(first.count, second.count));
    if ((first.count != 0U) || (second.count != 0U)) {
        status = toggle_led(BOARD_GPIO_EVB_BUTTON_LED);
        if (status != BS_STATUS_OK) {
            return status;
        }
    }
    if (first.overflowed || second.overflowed) {
        return BS_STATUS_OVERFLOW;
    }
    return BS_STATUS_OK;
}

uint32_t gpio_bringup_get_button_irq_count(void)
{
    /* 统计值由主循环处理事件时更新，不代表 ISR 内实时计数。 */
    return button_irq_count;
}

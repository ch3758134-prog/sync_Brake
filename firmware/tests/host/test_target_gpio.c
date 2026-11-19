#include "test_gpio.h"
#include "board_gpio_ids.h"
#include "drv_gpio.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

static uint32_t irq_enabled_count;
static uint32_t pending_button_1;
static uint32_t pending_button_2;
static bool led_heartbeat;
static bool led_button;

/* Fake Driver：模拟逻辑引脚，不访问 MCU。 */
bs_status_t drv_gpio_read(drv_gpio_id_t id, bool *level)
{
    if (level == NULL) { return BS_STATUS_INVALID_ARGUMENT; }
    if (id == BOARD_GPIO_EVB_HEARTBEAT_LED) { *level = led_heartbeat; return BS_STATUS_OK; }
    if (id == BOARD_GPIO_EVB_BUTTON_LED) { *level = led_button; return BS_STATUS_OK; }
    if (id == BOARD_GPIO_EVB_BUTTON_1) { *level = true; return BS_STATUS_OK; }
    if (id == BOARD_GPIO_EVB_BUTTON_2) { *level = false; return BS_STATUS_OK; }
    return BS_STATUS_INVALID_ARGUMENT;
}
bs_status_t drv_gpio_write(drv_gpio_id_t id, bool level)
{
    if (id == BOARD_GPIO_EVB_HEARTBEAT_LED) { led_heartbeat = level; return BS_STATUS_OK; }
    if (id == BOARD_GPIO_EVB_BUTTON_LED) { led_button = level; return BS_STATUS_OK; }
    return BS_STATUS_INVALID_ARGUMENT;
}
bs_status_t drv_gpio_set_irq_enabled(drv_gpio_id_t id, bool enabled)
{
    if ((id != BOARD_GPIO_EVB_BUTTON_1) && (id != BOARD_GPIO_EVB_BUTTON_2)) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (enabled) { ++irq_enabled_count; }
    return BS_STATUS_OK;
}
bs_status_t drv_gpio_take_events(drv_gpio_id_t id, drv_gpio_events_t *events)
{
    if (events == NULL) { return BS_STATUS_INVALID_ARGUMENT; }
    events->overflowed = false;
    if (id == BOARD_GPIO_EVB_BUTTON_1) {
        events->count = pending_button_1; pending_button_1 = 0U; return BS_STATUS_OK;
    }
    if (id == BOARD_GPIO_EVB_BUTTON_2) {
        events->count = pending_button_2; pending_button_2 = 0U; return BS_STATUS_OK;
    }
    return BS_STATUS_INVALID_ARGUMENT;
}
bool time_service_has_elapsed(uint32_t start_ms, uint32_t interval_ms, uint32_t now_ms)
{
    const uint32_t delta = now_ms - start_ms;
    return (interval_ms < UINT32_C(0x80000000)) &&
           (delta < UINT32_C(0x80000000)) &&
           (delta >= interval_ms);
}

int main(void)
{
    test_gpio_stats_t state = {0};
    assert(test_gpio_get_stats(&state) == BS_STATUS_NOT_INITIALIZED);
    assert(test_gpio_init(100U) == BS_STATUS_OK);
    assert(irq_enabled_count == 2U);
    assert(test_gpio_run_once(100U) == BS_STATUS_OK);
    assert(!led_heartbeat && !led_button);
    pending_button_1 = 2U;
    assert(test_gpio_run_once(600U) == BS_STATUS_OK);
    assert(led_heartbeat && led_button);
    assert(test_gpio_get_stats(&state) == BS_STATUS_OK);
    assert(state.processed_button_events == 2U);
    assert(state.poll_count == 2U);
    assert(state.button_1_level && !state.button_2_level);
    assert(test_gpio_get_stats(NULL) == BS_STATUS_INVALID_ARGUMENT);
    return 0;
}

#include "test_gpio.h"

#include "board_gpio_ids.h"
#include "drv_gpio.h"
#include "gpio_bringup.h"

#include <stddef.h>
#include <stdint.h>

/* 保留已有 gpio_bringup 的 LED/IRQ 回归测试逻辑，独立记录观测结果。 */
static test_gpio_stats_t s_test_gpio_stats;
static bool s_initialized;

bs_status_t test_gpio_init(uint32_t now_ms)
{
    bs_status_t status;
    s_initialized = false;
    s_test_gpio_stats = (test_gpio_stats_t){0};
    status = gpio_bringup_init(now_ms);
    s_test_gpio_stats.last_status = status;
    if (status == BS_STATUS_OK) {
        s_initialized = true;
    }
    return status;
}

bs_status_t test_gpio_run_once(uint32_t now_ms)
{
    bs_status_t status;
    bool button_1 = false;
    bool button_2 = false;

    if (!s_initialized) {
        return BS_STATUS_NOT_INITIALIZED;
    }
    status = gpio_bringup_poll(now_ms);
    if (status == BS_STATUS_OK) {
        status = drv_gpio_read(BOARD_GPIO_EVB_BUTTON_1, &button_1);
    }
    if (status == BS_STATUS_OK) {
        status = drv_gpio_read(BOARD_GPIO_EVB_BUTTON_2, &button_2);
    }
    if (status == BS_STATUS_OK) {
        s_test_gpio_stats.button_1_level = button_1;
        s_test_gpio_stats.button_2_level = button_2;
        s_test_gpio_stats.processed_button_events =
            gpio_bringup_get_button_irq_count();
        if (s_test_gpio_stats.poll_count != UINT32_MAX) {
            ++s_test_gpio_stats.poll_count;
        }
    }
    s_test_gpio_stats.last_status = status;
    return status;
}

bs_status_t test_gpio_get_stats(test_gpio_stats_t *out)
{
    if (out == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!s_initialized) {
        return BS_STATUS_NOT_INITIALIZED;
    }
    *out = s_test_gpio_stats;
    return BS_STATUS_OK;
}

#ifndef BRAKESYNC_TEST_GPIO_H
#define BRAKESYNC_TEST_GPIO_H
#include <stdbool.h>
#include <stdint.h>
#include "bs_status.h"

/* 仅供 EVB_NO_BRAKE_POWER 构建：每次调用的结果可在 Keil Watch 中观察。 */
typedef struct {
    uint32_t poll_count;
    uint32_t processed_button_events;
    bool button_1_level;
    bool button_2_level;
    bs_status_t last_status;
} test_gpio_stats_t;

bs_status_t test_gpio_init(uint32_t now_ms);
bs_status_t test_gpio_run_once(uint32_t now_ms);
bs_status_t test_gpio_get_stats(test_gpio_stats_t *out);

#endif

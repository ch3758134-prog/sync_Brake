#ifndef BRAKESYNC_TEST_TIMEBASE_H
#define BRAKESYNC_TEST_TIMEBASE_H
#include <stdbool.h>
#include <stdint.h>
#include "bs_status.h"

typedef struct {
    uint32_t poll_count;
    uint32_t completed_intervals;
    uint32_t missed_intervals;
    uint32_t last_now_ms;
    bs_status_t last_status;
} test_timebase_stats_t;

/* 仅测软件时基观测和 1s 周期统计；不宣称验证硬件计时精度。 */
bs_status_t test_timebase_init(uint32_t now_ms);
bs_status_t test_timebase_run_once(uint32_t now_ms);
bs_status_t test_timebase_get_stats(test_timebase_stats_t *out);

#endif

#include "test_timebase.h"

#include <stddef.h>
#include <stdint.h>

#define TEST_TIMEBASE_INTERVAL_MS UINT32_C(1000)
#define TEST_TIMEBASE_HALF_RANGE UINT32_C(0x80000000)

static test_timebase_stats_t s_stats;
static uint32_t s_previous_interval_ms;
static bool s_initialized;

static uint32_t sat_add(uint32_t a, uint32_t b)
{
    return (UINT32_MAX - a < b) ? UINT32_MAX : a + b;
}

bs_status_t test_timebase_init(uint32_t now_ms)
{
    s_previous_interval_ms = now_ms;
    s_stats = (test_timebase_stats_t){0};
    s_stats.last_now_ms = now_ms;
    s_stats.last_status = BS_STATUS_OK;
    s_initialized = true;
    return BS_STATUS_OK;
}

bs_status_t test_timebase_run_once(uint32_t now_ms)
{
    uint32_t elapsed;
    uint32_t intervals;

    if (!s_initialized) {
        return BS_STATUS_NOT_INITIALIZED;
    }
    /* 半量程内按 uint32_t 回绕比较；拒绝时间倒退或长时间停机的歧义。 */
    if ((now_ms - s_stats.last_now_ms) >= TEST_TIMEBASE_HALF_RANGE) {
        s_stats.last_status = BS_STATUS_DATA_INVALID;
        return BS_STATUS_DATA_INVALID;
    }
    elapsed = now_ms - s_previous_interval_ms;
    if (elapsed >= TEST_TIMEBASE_HALF_RANGE) {
        s_stats.last_status = BS_STATUS_DATA_INVALID;
        return BS_STATUS_DATA_INVALID;
    }
    intervals = elapsed / TEST_TIMEBASE_INTERVAL_MS;
    if (intervals > 0U) {
        s_stats.completed_intervals =
            sat_add(s_stats.completed_intervals, intervals);
        s_stats.missed_intervals =
            sat_add(s_stats.missed_intervals, intervals - 1U);
        s_previous_interval_ms += intervals * TEST_TIMEBASE_INTERVAL_MS;
    }
    if (s_stats.poll_count != UINT32_MAX) {
        ++s_stats.poll_count;
    }
    s_stats.last_now_ms = now_ms;
    s_stats.last_status = BS_STATUS_OK;
    return BS_STATUS_OK;
}

bs_status_t test_timebase_get_stats(test_timebase_stats_t *out)
{
    if (out == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!s_initialized) {
        return BS_STATUS_NOT_INITIALIZED;
    }
    *out = s_stats;
    return BS_STATUS_OK;
}

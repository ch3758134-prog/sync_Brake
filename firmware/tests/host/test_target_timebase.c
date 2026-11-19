#include "test_timebase.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

int main(void)
{
    test_timebase_stats_t result = {0};
    assert(test_timebase_get_stats(&result) == BS_STATUS_NOT_INITIALIZED);
    assert(test_timebase_init(100U) == BS_STATUS_OK);
    assert(test_timebase_run_once(1100U) == BS_STATUS_OK);
    assert(test_timebase_run_once(4200U) == BS_STATUS_OK);
    assert(test_timebase_get_stats(&result) == BS_STATUS_OK);
    assert(result.completed_intervals == 4U);
    assert(result.missed_intervals == 2U);
    assert(result.poll_count == 2U);
    assert(test_timebase_run_once(300U) == BS_STATUS_DATA_INVALID);
    assert(test_timebase_get_stats(&result) == BS_STATUS_OK);
    assert(result.last_status == BS_STATUS_DATA_INVALID);
    assert(test_timebase_get_stats(NULL) == BS_STATUS_INVALID_ARGUMENT);
    assert(test_timebase_init(UINT32_MAX - 4U) == BS_STATUS_OK);
    assert(test_timebase_run_once(1000U) == BS_STATUS_OK);
    assert(test_timebase_get_stats(&result) == BS_STATUS_OK);
    assert(result.completed_intervals == 1U);
    return 0;
}

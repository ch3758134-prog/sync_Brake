#include "motion_signal.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
int main(void)
{
    bs_motion_observation_t x = {0}, y = {0};
    motion_signal_policy_t p = {10U, true};
    bs_time_point_t now = {100U, 9U};
    x.quality = BS_DATA_VALID;
    x.received_at = (bs_time_point_t){99U, 9U};
    assert(motion_signal_evaluate(&x, &p, now, &y) == BS_API_INVALID_INPUT);
    assert(y.quality == BS_DATA_INVALID);
    x.mounting_verified = true;
    assert(motion_signal_evaluate(&x, &p, now, &y) == BS_API_OK);
    x.quality = BS_DATA_CONFLICT;
    assert(motion_signal_evaluate(&x, &p, now, &y) == BS_API_INVALID_INPUT);
    assert(y.quality == BS_DATA_CONFLICT);
    x.quality = BS_DATA_VALID;
    now.monotonic_ms = 112U;
    assert(motion_signal_evaluate(&x, &p, now, &y) == BS_API_INVALID_INPUT);
    assert(y.quality == BS_DATA_STALE);
    now.boot_id = 2U;
    assert(motion_signal_evaluate(&x, &p, now, &y) == BS_API_INVALID_TIME);
    x.received_at = (bs_time_point_t){UINT32_MAX - 2U, 9U};
    now = (bs_time_point_t){2U, 9U};
    assert(motion_signal_evaluate(&x, &p, now, &y) == BS_API_OK);
    assert(motion_signal_evaluate(&x, NULL, now, &y) == BS_API_INVALID_ARGUMENT);
    return 0;
}

#include "brake_intent.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
int main(void)
{
    bs_brake_intent_t x = {0}, y = {0};
    brake_intent_policy_t p = {10U, true};
    bs_time_point_t now = {100U, 7U};
    assert(brake_intent_evaluate(NULL, &p, now, &y) == BS_API_INVALID_ARGUMENT);
    x.state = BS_INTENT_RELEASED;
    x.quality = BS_DATA_VALID;
    x.observed_at = (bs_time_point_t){95U, 7U};
    assert(brake_intent_evaluate(&x, &p, now, &y) == BS_API_OK);
    assert(y.state == BS_INTENT_RELEASED);
    x.state = BS_INTENT_UNKNOWN;
    assert(brake_intent_evaluate(&x, &p, now, &y) == BS_API_INVALID_INPUT);
    assert(y.quality == BS_DATA_INVALID);
    x.state = BS_INTENT_APPLIED;
    now.monotonic_ms = 106U;
    assert(brake_intent_evaluate(&x, &p, now, &y) == BS_API_INVALID_INPUT);
    assert(y.quality == BS_DATA_STALE);
    now = (bs_time_point_t){96U, 8U};
    assert(brake_intent_evaluate(&x, &p, now, &y) == BS_API_INVALID_TIME);
    now = (bs_time_point_t){95U, 7U};
    p.source_verified = false;
    assert(brake_intent_evaluate(&x, &p, now, &y) == BS_API_INVALID_INPUT);
    p.source_verified = true;
    x.observed_at.monotonic_ms = UINT32_MAX - 2U;
    now.monotonic_ms = 2U;
    assert(brake_intent_evaluate(&x, &p, now, &y) == BS_API_OK);
    assert(y.state == BS_INTENT_APPLIED);
    return 0;
}

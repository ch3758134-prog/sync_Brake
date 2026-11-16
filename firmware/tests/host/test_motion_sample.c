#include "motion_sample.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int main(void)
{
    motion_sample_t sample = {0};

    assert(motion_sample_check(NULL, 1U, 100U, 10U)
           == MOTION_STATUS_INVALID_ARGUMENT);

    sample.boot_id = 1U;
    sample.quality = MOTION_QUALITY_VALID;
    sample.received_at_ms = 95U;
    assert(motion_sample_check(&sample, 1U, 100U, 10U)
           == MOTION_STATUS_OK);
    assert(motion_sample_check(&sample, 1U, 106U, 10U)
           == MOTION_STATUS_STALE);

    sample.quality = MOTION_QUALITY_SENSOR_ERROR;
    assert(motion_sample_check(&sample, 1U, 100U, 10U)
           == MOTION_STATUS_INVALID_DATA);

    sample.quality = MOTION_QUALITY_VALID;
    assert(motion_sample_check(&sample, 2U, 100U, 10U)
           == MOTION_STATUS_INVALID_TIME);
    assert(motion_sample_check(&sample, 1U, 90U, 10U)
           == MOTION_STATUS_INVALID_TIME);

    sample.received_at_ms = UINT32_MAX - 2U;
    assert(motion_sample_check(&sample, 1U, 2U, 10U)
           == MOTION_STATUS_OK);
    assert(motion_sample_check(&sample, 1U, 2U, UINT32_C(0x80000000))
           == MOTION_STATUS_INVALID_ARGUMENT);

    puts("Motion sample Host behavior tests: PASS");
    return 0;
}

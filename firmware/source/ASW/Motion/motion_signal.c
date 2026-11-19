#include "motion_signal.h"
#include <stddef.h>
#include <stdint.h>

bs_api_status_t motion_signal_evaluate(
    const bs_motion_observation_t *observation,
    const motion_signal_policy_t *policy,
    bs_time_point_t now, bs_motion_observation_t *result)
{
    uint32_t age_ms;
    if ((observation == NULL) || (policy == NULL) || (result == NULL) ||
        (policy->max_age_ms >= UINT32_C(0x80000000))) {
        return BS_API_INVALID_ARGUMENT;
    }
    *result = *observation;
    if (observation->received_at.boot_id != now.boot_id) {
        result->quality = BS_DATA_INVALID;
        return BS_API_INVALID_TIME;
    }
    age_ms = now.monotonic_ms - observation->received_at.monotonic_ms;
    if (age_ms >= UINT32_C(0x80000000)) {
        result->quality = BS_DATA_INVALID;
        return BS_API_INVALID_TIME;
    }
    if (age_ms > policy->max_age_ms) {
        result->quality = BS_DATA_STALE;
        return BS_API_INVALID_INPUT;
    }
    if (policy->orientation_required && !observation->mounting_verified) {
        result->quality = BS_DATA_INVALID;
        return BS_API_INVALID_INPUT;
    }
    if (observation->quality != BS_DATA_VALID) {
        if ((uint32_t)observation->quality > (uint32_t)BS_DATA_CONFLICT) {
            result->quality = BS_DATA_INVALID;
        }
        return BS_API_INVALID_INPUT;
    }
    return BS_API_OK;
}

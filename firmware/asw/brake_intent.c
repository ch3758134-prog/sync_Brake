#include "brake_intent.h"
#include <stddef.h>
#include <stdint.h>

bs_api_status_t brake_intent_evaluate(
    const bs_brake_intent_t *observation,
    const brake_intent_policy_t *policy,
    bs_time_point_t now, bs_brake_intent_t *result)
{
    uint32_t age_ms;
    if ((observation == NULL) || (policy == NULL) || (result == NULL) ||
        (policy->max_age_ms >= UINT32_C(0x80000000))) {
        return BS_API_INVALID_ARGUMENT;
    }
    *result = *observation;
    if (observation->observed_at.boot_id != now.boot_id) {
        result->quality = BS_DATA_INVALID;
        return BS_API_INVALID_TIME;
    }
    age_ms = now.monotonic_ms - observation->observed_at.monotonic_ms;
    if (age_ms >= UINT32_C(0x80000000)) {
        result->quality = BS_DATA_INVALID;
        return BS_API_INVALID_TIME;
    }
    if (age_ms > policy->max_age_ms) {
        result->quality = BS_DATA_STALE;
        return BS_API_INVALID_INPUT;
    }
    if (!policy->source_verified ||
        ((observation->state != BS_INTENT_RELEASED) &&
         (observation->state != BS_INTENT_APPLIED))) {
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

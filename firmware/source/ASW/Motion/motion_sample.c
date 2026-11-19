#include "motion_sample.h"

#include <stddef.h>
#include <stdint.h>

motion_status_t motion_sample_check(
    const motion_sample_t *sample,
    uint32_t current_boot_id,
    uint32_t now_ms,
    uint32_t max_age_ms)
{
    uint32_t elapsed_ms;

    if ((sample == NULL) || (max_age_ms >= UINT32_C(0x80000000)))
    {
        return MOTION_STATUS_INVALID_ARGUMENT;
    }

    if (sample->boot_id != current_boot_id)
    {
        return MOTION_STATUS_INVALID_TIME;
    }

    if (sample->quality != MOTION_QUALITY_VALID)
    {
        return MOTION_STATUS_INVALID_DATA;
    }

    /* 无符号回绕仅在半周期内可无歧义地计算时间差。 */
    elapsed_ms = now_ms - sample->received_at_ms;
    if (elapsed_ms >= UINT32_C(0x80000000))
    {
        return MOTION_STATUS_INVALID_TIME;
    }

    if (elapsed_ms > max_age_ms)
    {
        return MOTION_STATUS_STALE;
    }

    return MOTION_STATUS_OK;
}

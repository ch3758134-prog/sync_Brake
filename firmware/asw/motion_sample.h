#ifndef BRAKESYNC_MOTION_SAMPLE_H
#define BRAKESYNC_MOTION_SAMPLE_H

#include <stdint.h>

typedef enum
{
    MOTION_QUALITY_UNKNOWN = 0,
    MOTION_QUALITY_VALID,
    MOTION_QUALITY_SENSOR_ERROR
} motion_quality_t;

typedef enum
{
    MOTION_STATUS_OK = 0,
    MOTION_STATUS_INVALID_ARGUMENT,
    MOTION_STATUS_INVALID_DATA,
    MOTION_STATUS_INVALID_TIME,
    MOTION_STATUS_STALE
} motion_status_t;

typedef struct
{
    int32_t acceleration_mm_s2;
    uint32_t received_at_ms;
    uint32_t boot_id;
    motion_quality_t quality;
} motion_sample_t;

/* 检查接收时间和质量；不是传感器物理采样时间的完整验证。
 * 两个时间戳须同一 boot_id，时间差须小于无符号计数器半量程。
 */
motion_status_t motion_sample_check(
    const motion_sample_t *sample,
    uint32_t current_boot_id,
    uint32_t now_ms,
    uint32_t max_age_ms);

#endif

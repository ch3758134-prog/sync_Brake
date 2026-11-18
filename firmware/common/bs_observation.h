#ifndef BS_OBSERVATION_H
#define BS_OBSERVATION_H
#include <stdbool.h>
#include <stdint.h>
/* 此数据仅承担跨设备观测质量，不等于 ASW 控制输入。 */
typedef enum {
    BS_OBS_UNKNOWN = 0, BS_OBS_VALID, BS_OBS_INVALID,
    BS_OBS_STALE, BS_OBS_CONFLICT, BS_OBS_UNCALIBRATED
} bs_observation_quality_t;
typedef struct {
    uint16_t source_id;
    uint32_t boot_id;
    uint32_t received_at_ms; /* ECU 接收时刻；不是传感器采样时刻 */
    uint32_t sensor_tick;    /* 设备时基；未同步前不可与 ECU 时间相减 */
    uint32_t sequence;
    bool has_sensor_tick;
    bool has_sequence;
    bs_observation_quality_t quality;
    uint32_t reason_flags;  /* 各设备原因码暂不冻结；必须通过版本管理 */
} bs_observation_meta_t;
#endif

#ifndef BRAKESYNC_BS_CONTROL_CONTRACT_H
#define BRAKESYNC_BS_CONTROL_CONTRACT_H
#include <stdbool.h>
#include <stdint.h>
/* V0.2 语义合同，尚未定义真实执行功率和标定。 */
typedef struct { uint32_t monotonic_ms; uint32_t boot_id; } bs_time_point_t;
typedef enum {
    BS_DATA_UNKNOWN = 0, BS_DATA_VALID, BS_DATA_INVALID,
    BS_DATA_STALE, BS_DATA_CONFLICT
} bs_data_quality_t;
typedef enum {
    BS_INTENT_UNKNOWN = 0, BS_INTENT_RELEASED, BS_INTENT_APPLIED
} bs_intent_state_t;
typedef struct {
    bs_intent_state_t state;
    bs_data_quality_t quality;
    bs_time_point_t observed_at;
    uint16_t source_id;
    uint32_t reason_flags;
} bs_brake_intent_t;
typedef struct {
    int32_t longitudinal_accel_mm_s2;
    bs_data_quality_t quality;
    bs_time_point_t received_at; /* ECU 接收时间，非传感器物理采样时间 */
    uint32_t sensor_sample_tick;
    bool has_sensor_sample_tick;
    bool mounting_verified;
    uint16_t source_id;
    uint32_t reason_flags;
} bs_motion_observation_t;
typedef struct {
    uint16_t weight_profile_id;
    uint8_t force_level;
    uint32_t revision;
    bool valid;
    bool approved_for_control;
} bs_effective_config_t;
typedef enum {
    BS_RUN_INIT = 0, BS_RUN_INHIBITED, BS_RUN_READY,
    BS_RUN_ACTIVE, BS_RUN_DEGRADED, BS_RUN_FAULT, BS_RUN_SERVICE
} bs_run_state_t;
typedef struct {
    uint32_t snapshot_revision;
    bs_time_point_t snapshot_at;
    bs_brake_intent_t intent;
    bs_motion_observation_t motion;
    bs_effective_config_t config;
    bs_run_state_t run_state;
    bool logical_control_qualified;
    uint32_t active_fault_flags;
} bs_control_input_t;
typedef enum {
    BS_REQUEST_UNAVAILABLE = 0, BS_REQUEST_COMPUTED, BS_REQUEST_LIMITED
} bs_request_state_t;
typedef struct {
    uint32_t request_id;
    uint32_t source_snapshot_revision;
    bs_time_point_t decision_at;
    bs_request_state_t state;
    uint32_t reason_flags;
    bool has_demand;
    uint16_t logical_demand_permille; /* 待批准，非 PWM/电流 */
} bs_control_result_t;
typedef enum {
    BS_API_OK = 0, BS_API_INVALID_ARGUMENT, BS_API_NOT_INITIALIZED,
    BS_API_INVALID_TIME, BS_API_INVALID_INPUT, BS_API_NOT_READY
} bs_api_status_t;
#endif

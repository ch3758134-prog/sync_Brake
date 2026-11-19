#ifndef MOTION_SIGNAL_H
#define MOTION_SIGNAL_H
#include "bs_control_contract.h"
typedef struct {
    uint32_t max_age_ms;
    bool orientation_required;
} motion_signal_policy_t;
bs_api_status_t motion_signal_evaluate(
    const bs_motion_observation_t *observation,
    const motion_signal_policy_t *policy,
    bs_time_point_t now, bs_motion_observation_t *result);
#endif

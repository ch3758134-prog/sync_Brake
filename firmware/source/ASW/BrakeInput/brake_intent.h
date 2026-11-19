#ifndef BRAKE_INTENT_H
#define BRAKE_INTENT_H
#include "bs_control_contract.h"
typedef struct {
    uint32_t max_age_ms; /* 由已审查的产品时序策略提供，不给默认值 */
    bool source_verified;
} brake_intent_policy_t;
bs_api_status_t brake_intent_evaluate(
    const bs_brake_intent_t *observation,
    const brake_intent_policy_t *policy,
    bs_time_point_t now, bs_brake_intent_t *result);
#endif

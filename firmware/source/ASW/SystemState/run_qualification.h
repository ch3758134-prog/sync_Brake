#ifndef RUN_QUALIFICATION_H
#define RUN_QUALIFICATION_H
#include "bs_control_contract.h"
/* 输入为由 APP/系统验证得出的业务资格事实，不是 MCU 硬件状态的推断。 */
typedef struct {
    bool startup_complete;
    bool time_healthy;
    bool configuration_qualified;
    bool intent_available;
    bool motion_available;
    bool fault_blocking;
    uint32_t reason_flags;
} run_qualification_inputs_t;
typedef struct {
    bool logical_control_ready;  /* 不是实体制动许可 */
    bool degraded_candidate;     /* 安全分析前禁止自动宣布降级策略 */
    uint32_t reason_flags;
} run_qualification_result_t;
bs_api_status_t run_qualification_evaluate(
    const run_qualification_inputs_t *input,
    run_qualification_result_t *result);
#endif

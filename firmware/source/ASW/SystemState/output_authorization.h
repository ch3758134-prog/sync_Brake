#ifndef OUTPUT_AUTHORIZATION_H
#define OUTPUT_AUTHORIZATION_H
#include <stdbool.h>
#include <stdint.h>
#include "bs_status.h"
/* 仅供未来的受控执行设计评审；不导出 PWM、功率使能接口。 */
typedef struct {
    bool logical_request_available;
    bool software_state_qualified;
    bool input_fresh;
    uint32_t reason_flags;
} output_eligibility_inputs_t;
typedef struct {
    bool request_eligible_for_later_review;
    uint32_t reason_flags;
} output_eligibility_result_t;
bs_status_t output_authorization_evaluate_logic(
    const output_eligibility_inputs_t *input,
    output_eligibility_result_t *result);
#endif

#include "output_authorization.h"
#include <stddef.h>
bs_status_t output_authorization_evaluate_logic(
    const output_eligibility_inputs_t *input,
    output_eligibility_result_t *result)
{
    if ((input == NULL) || (result == NULL)) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    /* 只报告软件层完整性候选；永不代表硬件许可或安全断开能力。 */
    result->request_eligible_for_later_review =
        input->logical_request_available &&
        input->software_state_qualified &&
        input->input_fresh;
    result->reason_flags = input->reason_flags;
    return BS_STATUS_OK;
}

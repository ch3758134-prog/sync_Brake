#include "run_qualification.h"
#include <stddef.h>
bs_api_status_t run_qualification_evaluate(
    const run_qualification_inputs_t *input,
    run_qualification_result_t *result)
{
    if ((input == NULL) || (result == NULL)) {
        return BS_API_INVALID_ARGUMENT;
    }
    /* 保守的完整输入资格：缺少某一输入不能自行激活未批准的降级逻辑。 */
    result->logical_control_ready =
        input->startup_complete &&
        input->time_healthy &&
        input->configuration_qualified &&
        input->intent_available &&
        input->motion_available &&
        !input->fault_blocking;
    result->degraded_candidate = false;
    result->reason_flags = input->reason_flags;
    return BS_API_OK;
}

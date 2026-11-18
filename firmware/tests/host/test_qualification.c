#include "run_qualification.h"
#include "output_authorization.h"
#include <assert.h>
#include <stddef.h>
int main(void)
{
    run_qualification_inputs_t x = {true, true, true, true, true, false, 0U};
    run_qualification_result_t r = {0};
    output_eligibility_inputs_t y = {true, true, true, 0U};
    output_eligibility_result_t o = {0};

    assert(run_qualification_evaluate(NULL, &r) == BS_API_INVALID_ARGUMENT);
    assert(run_qualification_evaluate(&x, &r) == BS_API_OK);
    assert(r.logical_control_ready && !r.degraded_candidate);
    x.motion_available = false; x.reason_flags = 8U;
    assert(run_qualification_evaluate(&x, &r) == BS_API_OK);
    assert(!r.logical_control_ready && !r.degraded_candidate && r.reason_flags == 8U);
    x.motion_available = true; x.fault_blocking = true;
    assert(run_qualification_evaluate(&x, &r) == BS_API_OK);
    assert(!r.logical_control_ready);
    assert(output_authorization_evaluate_logic(NULL, &o) == BS_STATUS_INVALID_ARGUMENT);
    assert(output_authorization_evaluate_logic(&y, &o) == BS_STATUS_OK);
    assert(o.request_eligible_for_later_review);
    y.input_fresh = false; y.reason_flags = 6U;
    assert(output_authorization_evaluate_logic(&y, &o) == BS_STATUS_OK);
    assert(!o.request_eligible_for_later_review && o.reason_flags == 6U);
    return 0;
}

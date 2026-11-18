#include "diagnostics.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
int main(void)
{
    diagnostics_context_t c = {0};
    diagnostic_fault_t slots[2] = {{0}}, got = {0};
    diagnostic_fault_t f = {10U, 100U, 5U, true, true};
    assert(diagnostics_report(&c, &f) == BS_STATUS_NOT_INITIALIZED);
    assert(diagnostics_init(&c, slots, 2U) == BS_STATUS_OK);
    assert(diagnostics_get_fault(&c, 10U, &got) == BS_STATUS_EMPTY);
    assert(diagnostics_report(&c, &f) == BS_STATUS_OK);
    f.latched = false;
    f.changed_at_ms = 110U;
    assert(diagnostics_report(&c, &f) == BS_STATUS_OK);
    assert(diagnostics_get_fault(&c, 10U, &got) == BS_STATUS_OK);
    assert(got.latched && got.active && got.changed_at_ms == 110U);
    f.active = false;
    assert(diagnostics_report(&c, &f) == BS_STATUS_NOT_AUTHORIZED);
    f.active = true;
    f.fault_id = 11U;
    assert(diagnostics_report(&c, &f) == BS_STATUS_OK);
    f.fault_id = 12U;
    assert(diagnostics_report(&c, &f) == BS_STATUS_NO_RESOURCES);
    assert(c.used == 2U);
    return 0;
}

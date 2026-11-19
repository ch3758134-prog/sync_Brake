#include "diag_log.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

int main(void)
{
    diag_log_entry_t entry = {0};
    diag_log_entry_t got = {0};
    uint16_t pending = 0U;
    uint32_t dropped = 0U;
    uint32_t i;

    assert(diag_log_append(&entry) == BS_STATUS_NOT_INITIALIZED);
    assert(diag_log_init() == BS_STATUS_OK);
    assert(diag_log_take(&got) == BS_STATUS_EMPTY);
    assert(diag_log_take(NULL) == BS_STATUS_INVALID_ARGUMENT);

    for (i = 0U; i < 32U; ++i) {
        entry.event_id = i + 1U;
        entry.detail = i * 3U;
        assert(diag_log_append(&entry) == BS_STATUS_OK);
    }
    entry.event_id = 900U;
    assert(diag_log_append(&entry) == BS_STATUS_NO_RESOURCES);
    assert(diag_log_get_drop_count(&dropped) == BS_STATUS_OK);
    assert(dropped == 1U);
    assert(diag_log_get_pending_count(&pending) == BS_STATUS_OK);
    assert(pending == 32U);

    for (i = 0U; i < 16U; ++i) {
        assert(diag_log_take(&got) == BS_STATUS_OK);
        assert(got.event_id == i + 1U && got.detail == i * 3U);
    }
    for (i = 0U; i < 16U; ++i) {
        entry.event_id = 100U + i;
        assert(diag_log_append(&entry) == BS_STATUS_OK);
    }
    for (i = 16U; i < 32U; ++i) {
        assert(diag_log_take(&got) == BS_STATUS_OK);
        assert(got.event_id == i + 1U);
    }
    for (i = 0U; i < 16U; ++i) {
        assert(diag_log_take(&got) == BS_STATUS_OK);
        assert(got.event_id == 100U + i);
    }
    assert(diag_log_take(&got) == BS_STATUS_EMPTY);
    assert(diag_log_get_pending_count(&pending) == BS_STATUS_OK);
    assert(pending == 0U);
    assert(diag_log_append(NULL) == BS_STATUS_INVALID_ARGUMENT);
    assert(diag_log_get_drop_count(NULL) == BS_STATUS_INVALID_ARGUMENT);

    assert(diag_log_init() == BS_STATUS_OK);
    assert(diag_log_get_drop_count(&dropped) == BS_STATUS_OK);
    assert(dropped == 0U);
    return 0;
}

#include "app_scheduler.h"
#include <assert.h>
#include <stdint.h>
int main(void)
{
    app_scheduler_t s = {0};
    app_task_config_t tasks[2] = {
        {1U, 10U, 0U, APP_MISSED_REPORT_ONLY},
        {2U, 7U, 3U, APP_MISSED_SKIP}
    };
    app_task_state_t states[2] = {{0}};
    app_task_id_t id = 0U;
    assert(app_scheduler_take_due(&s, 0U, &id) == BS_STATUS_NOT_INITIALIZED);
    assert(app_scheduler_init(&s, tasks, states, 2U, 100U) == BS_STATUS_OK);
    assert(app_scheduler_take_due(&s, 100U, &id) == BS_STATUS_OK && id == 1U);
    assert(app_scheduler_take_due(&s, 100U, &id) == BS_STATUS_EMPTY);
    assert(app_scheduler_take_due(&s, 103U, &id) == BS_STATUS_OK && id == 2U);
    assert(app_scheduler_take_due(&s, 125U, &id) == BS_STATUS_OK && id == 1U);
    assert(states[0].missed_count == 1U);
    assert(app_scheduler_take_due(&s, 125U, &id) == BS_STATUS_EMPTY);
    assert(states[1].missed_count == 3U);
    tasks[1].id = tasks[0].id;
    assert(app_scheduler_init(&s, tasks, states, 2U, 0U) == BS_STATUS_INVALID_ARGUMENT);
    tasks[1].id = 2U;
    tasks[1].period_ms = 0U;
    assert(app_scheduler_init(&s, tasks, states, 2U, 0U) == BS_STATUS_INVALID_ARGUMENT);
    {
        app_scheduler_t rollover = {0};
        app_task_config_t task = {1U, 10U, 0U, APP_MISSED_REPORT_ONLY};
        app_task_state_t state = {0};
        assert(app_scheduler_init(&rollover, &task, &state, 1U, UINT32_MAX - 2U) == BS_STATUS_OK);
        assert(app_scheduler_take_due(&rollover, UINT32_MAX - 2U, &id) == BS_STATUS_OK);
        assert(app_scheduler_take_due(&rollover, 7U, &id) == BS_STATUS_OK);
    }
    return 0;
}

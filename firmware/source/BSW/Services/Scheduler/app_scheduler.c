#include "app_scheduler.h"
#include <stddef.h>
#include <stdint.h>
static uint32_t sat_add(uint32_t x, uint32_t y)
{
    return (UINT32_MAX - x < y) ? UINT32_MAX : x + y;
}
bs_status_t app_scheduler_init(app_scheduler_t *scheduler,
                               const app_task_config_t *tasks,
                               app_task_state_t *states,
                               uint8_t count, uint32_t now_ms)
{
    uint8_t i, j;
    if ((scheduler == NULL) || (tasks == NULL) || (states == NULL) ||
        (count == 0U)) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    for (i = 0U; i < count; ++i) {
        if ((tasks[i].period_ms == 0U) ||
            (tasks[i].period_ms >= UINT32_C(0x80000000)) ||
            (tasks[i].phase_ms >= UINT32_C(0x80000000)) ||
            ((tasks[i].missed_policy != APP_MISSED_SKIP) &&
             (tasks[i].missed_policy != APP_MISSED_REPORT_ONLY))) {
            return BS_STATUS_INVALID_ARGUMENT;
        }
        for (j = 0U; j < i; ++j) {
            if (tasks[i].id == tasks[j].id) {
                return BS_STATUS_INVALID_ARGUMENT;
            }
        }
    }
    for (i = 0U; i < count; ++i) {
        states[i].next_deadline_ms = now_ms + tasks[i].phase_ms;
        states[i].missed_count = 0U;
        states[i].initialized = true;
    }
    scheduler->tasks = tasks;
    scheduler->states = states;
    scheduler->count = count;
    scheduler->next_index = 0U;
    scheduler->initialized = true;
    return BS_STATUS_OK;
}
bs_status_t app_scheduler_take_due(app_scheduler_t *scheduler,
                                   uint32_t now_ms, app_task_id_t *task_id)
{
    uint8_t i, count, base;
    if ((scheduler == NULL) || (task_id == NULL)) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!scheduler->initialized || (scheduler->tasks == NULL) ||
        (scheduler->states == NULL) || (scheduler->count == 0U)) {
        return BS_STATUS_NOT_INITIALIZED;
    }
    count = scheduler->count;
    base = scheduler->next_index;
    for (i = 0U; i < count; ++i) {
        uint8_t idx = (uint8_t)(((uint16_t)base + i) % count);
        app_task_state_t *state = &scheduler->states[idx];
        const app_task_config_t *task = &scheduler->tasks[idx];
        uint32_t elapsed = now_ms - state->next_deadline_ms;
        uint32_t missed, cycles;
        if (elapsed >= UINT32_C(0x80000000)) {
            continue;
        }
        missed = elapsed / task->period_ms;
        cycles = missed + 1U;
        state->next_deadline_ms += cycles * task->period_ms;
        state->missed_count = sat_add(state->missed_count, missed);
        scheduler->next_index = (uint8_t)(((uint16_t)idx + 1U) % count);
        if ((missed > 0U) && (task->missed_policy == APP_MISSED_SKIP)) {
            state->missed_count = sat_add(state->missed_count, 1U);
            continue;
        }
        *task_id = task->id;
        return BS_STATUS_OK;
    }
    return BS_STATUS_EMPTY;
}

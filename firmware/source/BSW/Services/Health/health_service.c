#include "health_service.h"

#include <stddef.h>
#include <stdint.h>

#define HEALTH_SERVICE_MAX_SUBJECTS (8U)
#define HEALTH_TIME_HALF_RANGE UINT32_C(0x80000000)

typedef struct {
    health_subject_config_t config;
    health_observation_t latest;
    bool seen;
} health_subject_state_t;

static health_subject_state_t s_subjects[HEALTH_SERVICE_MAX_SUBJECTS];
static uint8_t s_subject_count;
static bool s_initialized;

static bool is_valid_role(health_subject_role_t role)
{
    return (role == HEALTH_ROLE_TASK) ||
           (role == HEALTH_ROLE_CLOCK) ||
           (role == HEALTH_ROLE_INPUT_QUALIFICATION);
}

bs_status_t health_service_init(const health_subject_config_t *subjects,
                                uint8_t count)
{
    uint8_t i;
    uint8_t j;

    if ((subjects == NULL) || (count == 0U) ||
        (count > HEALTH_SERVICE_MAX_SUBJECTS)) {
        return BS_STATUS_INVALID_ARGUMENT;
    }

    /* 校验完整输入后才替换既有设置，错误配置不可部分生效。 */
    for (i = 0U; i < count; ++i) {
        if (!is_valid_role(subjects[i].role) ||
            (subjects[i].max_age_ms == 0U) ||
            (subjects[i].max_age_ms >= HEALTH_TIME_HALF_RANGE)) {
            return BS_STATUS_INVALID_ARGUMENT;
        }
        for (j = 0U; j < i; ++j) {
            if (subjects[i].id == subjects[j].id) {
                return BS_STATUS_INVALID_ARGUMENT;
            }
        }
    }

    s_initialized = false;
    s_subject_count = count;
    for (i = 0U; i < HEALTH_SERVICE_MAX_SUBJECTS; ++i) {
        s_subjects[i] = (health_subject_state_t){0};
    }
    for (i = 0U; i < count; ++i) {
        s_subjects[i].config = subjects[i];
    }
    s_initialized = true;
    return BS_STATUS_OK;
}

bs_status_t health_report(const health_observation_t *observation)
{
    uint8_t i;

    if (observation == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!s_initialized) {
        return BS_STATUS_NOT_INITIALIZED;
    }
    if ((uint32_t)observation->condition > (uint32_t)HEALTH_UNKNOWN) {
        return BS_STATUS_INVALID_ARGUMENT;
    }

    for (i = 0U; i < s_subject_count; ++i) {
        health_subject_state_t *subject = &s_subjects[i];
        if (subject->config.id == observation->id) {
            if (subject->seen &&
                ((observation->observed_at_ms -
                  subject->latest.observed_at_ms) >= HEALTH_TIME_HALF_RANGE)) {
                return BS_STATUS_DATA_INVALID;
            }
            subject->latest = *observation;
            subject->seen = true;
            return BS_STATUS_OK;
        }
    }
    return BS_STATUS_INVALID_ARGUMENT;
}

bs_status_t health_get_snapshot(uint32_t now_ms, health_snapshot_t *snapshot)
{
    uint8_t i;
    bool seen_task = false;
    bool seen_clock = false;
    bool seen_input = false;

    if (snapshot == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!s_initialized) {
        return BS_STATUS_NOT_INITIALIZED;
    }

    /* 没有配置某个角色时，该角色不能被默认为健康。 */
    *snapshot = (health_snapshot_t){false, false, false, now_ms};
    snapshot->monitored_tasks_healthy = true;
    snapshot->clock_healthy = true;
    snapshot->input_qualification_healthy = true;

    for (i = 0U; i < s_subject_count; ++i) {
        const health_subject_state_t *subject = &s_subjects[i];
        bool healthy = subject->seen;
        if (healthy) {
            uint32_t age_ms = now_ms - subject->latest.observed_at_ms;
            healthy = (age_ms < HEALTH_TIME_HALF_RANGE) &&
                      (age_ms <= subject->config.max_age_ms) &&
                      (subject->latest.condition == HEALTH_OK);
        }
        switch (subject->config.role) {
        case HEALTH_ROLE_TASK:
            seen_task = true;
            snapshot->monitored_tasks_healthy =
                snapshot->monitored_tasks_healthy && healthy;
            break;
        case HEALTH_ROLE_CLOCK:
            seen_clock = true;
            snapshot->clock_healthy = snapshot->clock_healthy && healthy;
            break;
        case HEALTH_ROLE_INPUT_QUALIFICATION:
            seen_input = true;
            snapshot->input_qualification_healthy =
                snapshot->input_qualification_healthy && healthy;
            break;
        default:
            /* init 已拒绝非法角色；保持保守的全否定结果。 */
            snapshot->monitored_tasks_healthy = false;
            snapshot->clock_healthy = false;
            snapshot->input_qualification_healthy = false;
            break;
        }
    }

    snapshot->monitored_tasks_healthy =
        snapshot->monitored_tasks_healthy && seen_task;
    snapshot->clock_healthy = snapshot->clock_healthy && seen_clock;
    snapshot->input_qualification_healthy =
        snapshot->input_qualification_healthy && seen_input;
    return BS_STATUS_OK;
}

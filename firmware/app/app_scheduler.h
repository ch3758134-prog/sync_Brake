#ifndef APP_SCHEDULER_H
#define APP_SCHEDULER_H
#include <stdbool.h>
#include <stdint.h>
#include "bs_status.h"
typedef uint8_t app_task_id_t;
typedef enum { APP_MISSED_SKIP = 0, APP_MISSED_REPORT_ONLY } app_missed_policy_t;
typedef struct {
    app_task_id_t id;
    uint32_t period_ms;
    uint32_t phase_ms;
    app_missed_policy_t missed_policy;
} app_task_config_t;
typedef struct {
    uint32_t next_deadline_ms;
    uint32_t missed_count;
    bool initialized;
} app_task_state_t;
typedef struct {
    const app_task_config_t *tasks;
    app_task_state_t *states;
    uint8_t count;
    uint8_t next_index;
    bool initialized;
} app_scheduler_t;
bs_status_t app_scheduler_init(app_scheduler_t *scheduler,
                               const app_task_config_t *tasks,
                               app_task_state_t *states,
                               uint8_t count, uint32_t now_ms);
bs_status_t app_scheduler_take_due(app_scheduler_t *scheduler,
                                   uint32_t now_ms, app_task_id_t *task_id);
#endif

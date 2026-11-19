#ifndef HEALTH_SERVICE_H
#define HEALTH_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include "bs_status.h"

typedef uint16_t health_subject_id_t;
typedef enum {
    HEALTH_OK = 0,
    HEALTH_LATE,
    HEALTH_FAILED,
    HEALTH_UNKNOWN
} health_condition_t;
typedef enum {
    HEALTH_ROLE_TASK = 0,
    HEALTH_ROLE_CLOCK,
    HEALTH_ROLE_INPUT_QUALIFICATION
} health_subject_role_t;

typedef struct {
    health_subject_id_t id;
    health_subject_role_t role;
    uint32_t max_age_ms;  /* 来自已审查任务时序预算，不设默认值 */
} health_subject_config_t;
typedef struct {
    health_subject_id_t id;
    health_condition_t condition;
    uint32_t observed_at_ms; /* 与健康快照使用同一启动代次的时基 */
} health_observation_t;
typedef struct {
    bool monitored_tasks_healthy;
    bool clock_healthy;
    bool input_qualification_healthy;
    uint32_t checked_at_ms;
} health_snapshot_t;

/* V0.2 候审接口的细化：明确监督对象和时间预算来源。
 * 非中断单执行上下文；最多 8 个监督对象，不使用动态内存。
 */
bs_status_t health_service_init(const health_subject_config_t *subjects,
                                uint8_t count);
bs_status_t health_report(const health_observation_t *observation);
bs_status_t health_get_snapshot(uint32_t now_ms, health_snapshot_t *snapshot);

/* 不开放 feed_watchdog；健康汇总不能单独授权实际制动输出。 */
#endif

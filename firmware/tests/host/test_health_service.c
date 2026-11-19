#include "health_service.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

int main(void)
{
    health_snapshot_t status = {0};
    health_subject_config_t subjects[3] = {
        {1U, HEALTH_ROLE_TASK, 20U},
        {2U, HEALTH_ROLE_CLOCK, 50U},
        {3U, HEALTH_ROLE_INPUT_QUALIFICATION, 80U}
    };
    health_observation_t obs = {1U, HEALTH_OK, 100U};

    assert(health_get_snapshot(100U, &status) == BS_STATUS_NOT_INITIALIZED);
    assert(health_service_init(NULL, 3U) == BS_STATUS_INVALID_ARGUMENT);
    assert(health_service_init(subjects, 0U) == BS_STATUS_INVALID_ARGUMENT);
    subjects[2].id = subjects[1].id;
    assert(health_service_init(subjects, 3U) == BS_STATUS_INVALID_ARGUMENT);
    subjects[2].id = 3U;
    subjects[1].max_age_ms = 0U;
    assert(health_service_init(subjects, 3U) == BS_STATUS_INVALID_ARGUMENT);
    subjects[1].max_age_ms = 50U;
    assert(health_service_init(subjects, 3U) == BS_STATUS_OK);
    assert(health_get_snapshot(100U, &status) == BS_STATUS_OK);
    assert(!status.monitored_tasks_healthy &&
           !status.clock_healthy &&
           !status.input_qualification_healthy);

    assert(health_report(&obs) == BS_STATUS_OK);
    obs.id = 2U;
    assert(health_report(&obs) == BS_STATUS_OK);
    obs.id = 3U;
    assert(health_report(&obs) == BS_STATUS_OK);
    assert(health_get_snapshot(100U, &status) == BS_STATUS_OK);
    assert(status.monitored_tasks_healthy && status.clock_healthy &&
           status.input_qualification_healthy);

    assert(health_get_snapshot(121U, &status) == BS_STATUS_OK);
    assert(!status.monitored_tasks_healthy && status.clock_healthy &&
           status.input_qualification_healthy);
    obs.id = 1U;
    obs.observed_at_ms = 125U;
    assert(health_report(&obs) == BS_STATUS_OK);
    assert(health_get_snapshot(125U, &status) == BS_STATUS_OK);
    assert(status.monitored_tasks_healthy);

    obs.id = 3U;
    obs.condition = HEALTH_FAILED;
    obs.observed_at_ms = 126U;
    assert(health_report(&obs) == BS_STATUS_OK);
    assert(health_get_snapshot(126U, &status) == BS_STATUS_OK);
    assert(!status.input_qualification_healthy);
    obs.id = 1000U;
    assert(health_report(&obs) == BS_STATUS_INVALID_ARGUMENT);
    obs.id = 1U;
    obs.condition = HEALTH_OK;
    obs.observed_at_ms = 1U;
    assert(health_report(&obs) == BS_STATUS_DATA_INVALID);

    assert(health_service_init(subjects, 3U) == BS_STATUS_OK);
    obs.id = 1U;
    obs.observed_at_ms = UINT32_MAX - 5U;
    assert(health_report(&obs) == BS_STATUS_OK);
    obs.id = 2U;
    assert(health_report(&obs) == BS_STATUS_OK);
    obs.id = 3U;
    assert(health_report(&obs) == BS_STATUS_OK);
    assert(health_get_snapshot(2U, &status) == BS_STATUS_OK);
    assert(status.monitored_tasks_healthy && status.clock_healthy &&
           status.input_qualification_healthy);

    assert(health_get_snapshot(2U, NULL) == BS_STATUS_INVALID_ARGUMENT);
    return 0;
}

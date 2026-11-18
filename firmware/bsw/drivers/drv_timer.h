#ifndef DRV_TIMER_H
#define DRV_TIMER_H
#include "drv_types.h"
typedef struct {
    uint32_t min_period_us;
    uint32_t max_period_us;
    uint32_t nominal_resolution_us;
    bool periodic_supported;
} drv_timer_capabilities_t;
typedef struct {
    uint32_t expirations;
    bool overflowed;
} drv_timer_events_t;
bs_status_t drv_timer_get_capabilities(drv_timer_id_t id,
                                       drv_timer_capabilities_t *caps);
/* 时间精度、配置离散化规则必须在适配实现中报告，不得静默取近似值。 */
bs_status_t drv_timer_start_periodic(drv_timer_id_t id, uint32_t period_us);
bs_status_t drv_timer_stop(drv_timer_id_t id);
bs_status_t drv_timer_take_events(drv_timer_id_t id, drv_timer_events_t *events);
#endif

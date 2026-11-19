#ifndef DRV_CLOCK_H
#define DRV_CLOCK_H
#include <stdbool.h>
#include <stdint.h>
#include "bs_status.h"
/* Clock 初始化仍由 BoardCfg 完成；通用接口只报告已核实的时钟事实。 */
typedef enum {
    DRV_CLOCK_CORE = 0,
    DRV_CLOCK_FAST_BUS,
    DRV_CLOCK_SLOW_BUS
} drv_clock_domain_t;
typedef struct {
    uint32_t frequency_hz;
    bool valid;
    bool fault_observed;
} drv_clock_status_t;
bs_status_t drv_clock_get_status(drv_clock_domain_t domain,
                                 drv_clock_status_t *status);
#endif

#ifndef DRV_RESET_H
#define DRV_RESET_H
#include <stdint.h>
#include <stdbool.h>
#include "bs_status.h"
/* 硬件支持哪些复位源及复位标志清除规则，由对应 Target 审查。 */
typedef struct {
    uint32_t raw_cause;     /* 厂商原始位域仅供记录，跨 MCU 不比较 */
    uint32_t normalized_causes; /* 规范化位定义待批准后增加 */
    bool captured_at_startup;
} drv_reset_info_t;
bs_status_t drv_reset_get_startup_info(drv_reset_info_t *info);
#endif

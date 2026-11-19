#ifndef DIAG_LOG_H
#define DIAG_LOG_H

#include <stdint.h>
#include "bs_status.h"

/* 有界事件历史：不替代 ASW 诊断状态机和已锁存故障。 */
typedef struct {
    uint32_t event_id;
    uint32_t occurred_at_ms;
    uint16_t source_id;
    uint16_t event_code;
    uint32_t detail;
} diag_log_entry_t;

/* 单实例、主循环上下文；不允许中断直接调用。
 * 初始化会清空非持久日志，NVM 持久化需单独设计。
 */
bs_status_t diag_log_init(void);
bs_status_t diag_log_append(const diag_log_entry_t *entry);
bs_status_t diag_log_take(diag_log_entry_t *entry);
bs_status_t diag_log_get_drop_count(uint32_t *dropped);
bs_status_t diag_log_get_pending_count(uint16_t *pending);

#endif

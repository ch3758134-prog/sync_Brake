#include "diag_log.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 32 条 * 16 字节 = 512 字节静态存储；调整需重新审查 MCU RAM 预算。 */
#define DIAG_LOG_CAPACITY (32U)

static diag_log_entry_t s_entries[DIAG_LOG_CAPACITY];
static uint16_t s_head;
static uint16_t s_tail;
static uint16_t s_count;
static uint32_t s_dropped;
static bool s_initialized;

bs_status_t diag_log_init(void)
{
    uint16_t i;
    s_initialized = false;
    for (i = 0U; i < DIAG_LOG_CAPACITY; ++i) {
        s_entries[i] = (diag_log_entry_t){0};
    }
    s_head = 0U;
    s_tail = 0U;
    s_count = 0U;
    s_dropped = 0U;
    s_initialized = true;
    return BS_STATUS_OK;
}

bs_status_t diag_log_append(const diag_log_entry_t *entry)
{
    if (entry == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!s_initialized) {
        return BS_STATUS_NOT_INITIALIZED;
    }
    /* 满时保留旧记录，报告本次丢失，不冒充写入成功。 */
    if (s_count == DIAG_LOG_CAPACITY) {
        if (s_dropped != UINT32_MAX) {
            ++s_dropped;
        }
        return BS_STATUS_NO_RESOURCES;
    }
    s_entries[s_tail] = *entry;
    s_tail = (uint16_t)((s_tail + 1U) % DIAG_LOG_CAPACITY);
    ++s_count;
    return BS_STATUS_OK;
}

bs_status_t diag_log_take(diag_log_entry_t *entry)
{
    if (entry == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!s_initialized) {
        return BS_STATUS_NOT_INITIALIZED;
    }
    if (s_count == 0U) {
        return BS_STATUS_EMPTY;
    }

    *entry = s_entries[s_head];
    s_head = (uint16_t)((s_head + 1U) % DIAG_LOG_CAPACITY);
    --s_count;
    return BS_STATUS_OK;
}

bs_status_t diag_log_get_drop_count(uint32_t *dropped)
{
    if (dropped == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!s_initialized) {
        return BS_STATUS_NOT_INITIALIZED;
    }
    *dropped = s_dropped;
    return BS_STATUS_OK;
}

bs_status_t diag_log_get_pending_count(uint16_t *pending)
{
    if (pending == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!s_initialized) {
        return BS_STATUS_NOT_INITIALIZED;
    }
    *pending = s_count;
    return BS_STATUS_OK;
}

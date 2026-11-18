#include "diagnostics.h"
#include <stddef.h>
#include <stdint.h>
bs_status_t diagnostics_init(diagnostics_context_t *ctx,
                             diagnostic_fault_t *storage, uint16_t capacity)
{
    uint16_t i;
    if ((ctx == NULL) || (storage == NULL) || (capacity == 0U)) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    for (i = 0U; i < capacity; ++i) {
        storage[i] = (diagnostic_fault_t){0};
    }
    ctx->storage = storage;
    ctx->capacity = capacity;
    ctx->used = 0U;
    ctx->initialized = true;
    return BS_STATUS_OK;
}
bs_status_t diagnostics_report(diagnostics_context_t *ctx,
                               const diagnostic_fault_t *fault)
{
    uint16_t i;
    if ((ctx == NULL) || (fault == NULL)) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!ctx->initialized || (ctx->storage == NULL)) {
        return BS_STATUS_NOT_INITIALIZED;
    }
    /* 无单独恢复证据接口，不允许以事件报告操作清除活动故障。 */
    if (!fault->active) {
        return BS_STATUS_NOT_AUTHORIZED;
    }
    for (i = 0U; i < ctx->used; ++i) {
        if (ctx->storage[i].fault_id == fault->fault_id) {
            diagnostic_fault_t updated = *fault;
            updated.latched = updated.latched || ctx->storage[i].latched;
            ctx->storage[i] = updated;
            return BS_STATUS_OK;
        }
    }
    if (ctx->used >= ctx->capacity) {
        return BS_STATUS_NO_RESOURCES;
    }
    ctx->storage[ctx->used++] = *fault;
    return BS_STATUS_OK;
}
bs_status_t diagnostics_get_fault(const diagnostics_context_t *ctx,
                                  diagnostic_fault_id_t id,
                                  diagnostic_fault_t *fault)
{
    uint16_t i;
    if ((ctx == NULL) || (fault == NULL)) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (!ctx->initialized || (ctx->storage == NULL)) {
        return BS_STATUS_NOT_INITIALIZED;
    }
    for (i = 0U; i < ctx->used; ++i) {
        if (ctx->storage[i].fault_id == id) {
            *fault = ctx->storage[i];
            return BS_STATUS_OK;
        }
    }
    return BS_STATUS_EMPTY;
}

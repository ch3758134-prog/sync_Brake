#ifndef ASW_DIAGNOSTICS_H
#define ASW_DIAGNOSTICS_H
#include <stdbool.h>
#include <stdint.h>
#include "bs_status.h"
typedef uint16_t diagnostic_fault_id_t;
typedef struct {
    diagnostic_fault_id_t fault_id;
    uint32_t changed_at_ms;
    uint32_t reason;
    bool active;
    bool latched;
} diagnostic_fault_t;
typedef struct {
    diagnostic_fault_t *storage;
    uint16_t capacity;
    uint16_t used;
    bool initialized;
} diagnostics_context_t;
bs_status_t diagnostics_init(diagnostics_context_t *ctx,
                             diagnostic_fault_t *storage, uint16_t capacity);
bs_status_t diagnostics_report(diagnostics_context_t *ctx,
                               const diagnostic_fault_t *fault);
bs_status_t diagnostics_get_fault(const diagnostics_context_t *ctx,
                                  diagnostic_fault_id_t id,
                                  diagnostic_fault_t *fault);
#endif

#include "mcal_clock.h"
#include "clock.h"

uint32_t MCAL_CLOCK_GetFrequency(mcal_clock_domain_t domain)
{
    clock_names_t name;
    uint32_t hz = 0U;
    switch (domain) {
    case MCAL_CLOCK_CORE:     name = CORE_CLK; break;
    case MCAL_CLOCK_FAST_BUS: name = FAST_BUS_CLK; break;
    case MCAL_CLOCK_SLOW_BUS: name = SLOW_BUS_CLK; break;
    case MCAL_CLOCK_UART1:    name = UART1_CLK; break;
    default: return 0U;
    }
    return (CLOCK_SYS_GetFreq(name, &hz) == STATUS_SUCCESS) ? hz : 0U;
}
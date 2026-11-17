#include "time_service.h"

#include "mcal_time.h"

void time_service_init(void)
{
    mcal_time_init();
}

uint32_t time_service_now_ms(void)
{
    return mcal_time_now_ms();
}

bool time_service_has_elapsed(uint32_t start_ms,
                              uint32_t interval_ms,
                              uint32_t now_ms)
{
    const uint32_t elapsed_ms = now_ms - start_ms;

    if (interval_ms >= UINT32_C(0x80000000))
    {
        return false;
    }

    /* 半周期以上的时间差可能意味着时间戳无效或属于旧启动代次。 */
    if (elapsed_ms >= UINT32_C(0x80000000))
    {
        return false;
    }

    return (elapsed_ms >= interval_ms);
}

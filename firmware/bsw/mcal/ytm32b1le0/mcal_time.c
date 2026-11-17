#include "mcal_time.h"
#include "osif.h"

void mcal_time_init(void)
{
    /* 此 SDK 的裸机 OSIF 需要先调用 Delay(0) 才初始化 SysTick。
     * 依据：osif_baremetal.c 中 OSIF_GetMilliseconds 的接口说明。
     */
    OSIF_TimeDelay(0U);
}

uint32_t mcal_time_now_ms(void)
{
    return OSIF_GetMilliseconds();
}

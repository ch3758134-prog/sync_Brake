#ifndef BRAKESYNC_MCAL_CLOCK_H
#define BRAKESYNC_MCAL_CLOCK_H
#include <stdint.h>
/* YTM32B1LE0 时钟查询。系统时钟初始化统一由 system_init() 完成。 */
typedef enum {
    MCAL_CLOCK_CORE = 0,
    MCAL_CLOCK_FAST_BUS,
    MCAL_CLOCK_SLOW_BUS,
    MCAL_CLOCK_UART1
} mcal_clock_domain_t;
/* 时钟未就绪、门控关闭或参数无效返回 0Hz；不缓存时钟频率。 */
uint32_t MCAL_CLOCK_GetFrequency(mcal_clock_domain_t domain);
#endif
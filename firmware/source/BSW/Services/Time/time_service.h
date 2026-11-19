#ifndef BRAKESYNC_TIME_SERVICE_H
#define BRAKESYNC_TIME_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

/* 提供独立于 MCU 的软件时间接口；调用方需确保同一启动代次。 */
void time_service_init(void);
uint32_t time_service_now_ms(void);

/* 仅用于时间跨度小于 2^31 ms 的同一启动代次。
 * 参数错误或检测到逆向/歧义时间差时返回 false。
 */
bool time_service_has_elapsed(uint32_t start_ms,
                              uint32_t interval_ms,
                              uint32_t now_ms);

#endif

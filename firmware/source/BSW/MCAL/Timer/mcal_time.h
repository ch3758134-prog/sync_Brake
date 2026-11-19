#ifndef BRAKESYNC_MCAL_TIME_H
#define BRAKESYNC_MCAL_TIME_H

#include <stdint.h>

/* 初始化 SDK 单调时基。必须在 Board 时钟配置完成后调用。 */
void mcal_time_init(void);

/* 返回本次 MCU 运行周期内的毫秒计数；按 uint32_t 回绕。 */
uint32_t mcal_time_now_ms(void);

#endif

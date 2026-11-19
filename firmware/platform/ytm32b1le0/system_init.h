#ifndef BRAKESYNC_SYSTEM_INIT_H
#define BRAKESYNC_SYSTEM_INIT_H

typedef enum
{
    SYSTEM_INIT_OK = 0,
    SYSTEM_INIT_CLOCK_ERROR,
    SYSTEM_INIT_PINS_ERROR
} system_init_status_t;

/* 只初始化当前 YTM32B1LE05 EVB 的时钟和 PinMux。
 * 当前工程未连接、未实现真实制动功率路径。
 */
system_init_status_t system_init(void);

#endif

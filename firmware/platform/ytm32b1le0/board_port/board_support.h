#ifndef BRAKESYNC_BOARD_SUPPORT_H
#define BRAKESYNC_BOARD_SUPPORT_H

typedef enum
{
    BOARD_STATUS_OK = 0,
    BOARD_STATUS_CLOCK_ERROR,
    BOARD_STATUS_PINS_ERROR
} board_status_t;

/* 只初始化当前 YTM32B1LE05 EVB 的时钟和 PinMux。
 * 当前工程未连接、未实现真实制动功率路径。
 */
board_status_t board_support_init(void);

#endif

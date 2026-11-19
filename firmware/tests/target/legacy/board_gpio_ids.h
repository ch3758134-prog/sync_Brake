#ifndef BOARD_GPIO_IDS_H
#define BOARD_GPIO_IDS_H

/* 仅用于当前 EVB 的逻辑资源映射；值不是 MCU 物理引脚号。 */
enum {
    BOARD_GPIO_EVB_HEARTBEAT_LED = 1,
    BOARD_GPIO_EVB_BUTTON_LED = 2,
    BOARD_GPIO_EVB_BUTTON_1 = 3,
    BOARD_GPIO_EVB_BUTTON_2 = 4
};

#endif

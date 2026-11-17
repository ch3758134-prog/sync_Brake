#ifndef CAN_BUS_BOARD_H
#define CAN_BUS_BOARD_H

#include "can_bus.h"

typedef enum
{
    CAN_BOARD_OK = 0,
    CAN_BOARD_BUSY,
    CAN_BOARD_BUS_OFF,
    CAN_BOARD_ERROR
} can_board_result_t;

/* 仅在云途板级文件中实现；不得在正式 Target 构建中链接 Host 假实现。 */
can_board_result_t can_board_init(void);
can_board_result_t can_board_start(void);
can_board_result_t can_board_send(const can_frame_t *frame);
can_board_result_t can_board_recover(void);

/* 屏蔽全部 CAN 回调相关 IRQ，返回原中断状态并在退出时精确恢复。
 * 实现必须保证编译器/CPU 访问顺序，不能仅依靠 volatile。
 */
uint32_t can_board_enter_critical(void);
void can_board_exit_critical(uint32_t previous_state);

#endif

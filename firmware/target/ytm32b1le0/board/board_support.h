#ifndef BRAKESYNC_BOARD_SUPPORT_H
#define BRAKESYNC_BOARD_SUPPORT_H

#include <stdint.h>

typedef enum
{
    BOARD_STATUS_OK = 0,
    BOARD_STATUS_CLOCK_ERROR,
    BOARD_STATUS_PINS_ERROR
} board_status_t;

/* 本文件只面向当前云途开发板，不代表未来产品 PCB 配置。 */
board_status_t board_support_init(void);
void board_support_toggle_heartbeat(void);
uint32_t board_support_get_button_irq_count(void);

#endif

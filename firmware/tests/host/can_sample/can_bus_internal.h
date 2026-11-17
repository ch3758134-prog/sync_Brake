#ifndef CAN_BUS_INTERNAL_H
#define CAN_BUS_INTERNAL_H

#include "can_bus.h"

/* 只给板级 SDK 回调使用。ISR 必须完成 SDK 要求的标志清除和接收重装。 */
void can_bus_on_rx_isr(const can_frame_t *frame);
void can_bus_on_tx_done_isr(bool successful);
void can_bus_on_bus_off_isr(void);

#endif

#ifndef DRV_CAN_PORT_H
#define DRV_CAN_PORT_H
/* BSW 内部：选用一份 MCU/Host 实现，业务和 CDD 禁止包含。 */
#include "drv_can.h"
bs_status_t drv_can_port_init(drv_can_id_t id);
bs_status_t drv_can_port_submit(drv_can_id_t id,
                                const drv_can_frame_t *frame,
                                uint32_t request_id);
bs_status_t drv_can_port_get_hw_state(drv_can_id_t id,
                                      drv_can_state_t *state);
bs_status_t drv_can_port_request_recovery(drv_can_id_t id);
/* IRQ/SDK 回调中通知公共核心；只记录事件或有界复制帧。 */
void drv_can_core_on_rx_isr(drv_can_id_t id, const drv_can_rx_t *rx);
void drv_can_core_on_tx_isr(drv_can_id_t id, uint32_t request_id,
                            bool completed, uint32_t observed_at_ms);
void drv_can_core_on_bus_fault_isr(drv_can_id_t id, bool bus_off,
                                   uint32_t observed_at_ms);
#endif

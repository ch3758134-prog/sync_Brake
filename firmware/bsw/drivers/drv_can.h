#ifndef DRV_CAN_H
#define DRV_CAN_H
#include "drv_types.h"
typedef enum {
    DRV_CAN_ID_STANDARD = 0, DRV_CAN_ID_EXTENDED
} drv_can_id_format_t;
typedef struct {
    uint32_t arbitration_id;
    uint8_t data[8];
    uint8_t length; /* 0..8; 首版仅 Classic CAN 数据帧 */
    drv_can_id_format_t id_format;
} drv_can_frame_t;
typedef struct {
    drv_can_frame_t frame;
    uint32_t received_at_ms;
    bool timestamp_valid;
} drv_can_rx_t;
typedef enum {
    DRV_CAN_EVENT_TX_COMPLETED = 0,
    DRV_CAN_EVENT_TX_FAILED,
    DRV_CAN_EVENT_BUS_OFF,
    DRV_CAN_EVENT_RECOVERED,
    DRV_CAN_EVENT_RX_OVERFLOW
} drv_can_event_kind_t;
typedef struct {
    drv_can_event_kind_t kind;
    uint32_t request_id;
    uint32_t observed_at_ms;
    uint32_t detail;
} drv_can_event_t;
typedef enum {
    DRV_CAN_STATE_UNINITIALIZED = 0,
    DRV_CAN_STATE_READY,
    DRV_CAN_STATE_BUS_OFF,
    DRV_CAN_STATE_FAULT
} drv_can_state_t;
typedef struct {
    drv_can_state_t state;
    uint32_t rx_count;
    uint32_t rx_dropped;
    uint32_t tx_completed;
    uint32_t tx_failed;
    uint32_t bus_off_count;
} drv_can_status_t;
typedef struct {
    bool classic_can_supported;
    bool extended_id_supported;
    uint8_t rx_queue_capacity;
    uint8_t tx_queue_capacity;
} drv_can_capabilities_t;
/* 纯数据校验，可在 Host 与 MCU 上使用；不代表总线已初始化。 */
bs_status_t drv_can_frame_validate(const drv_can_frame_t *frame);
bs_status_t drv_can_init(drv_can_id_t id);
bs_status_t drv_can_get_capabilities(drv_can_id_t id,
                                     drv_can_capabilities_t *caps);
/* OK 表示整帧已复制入有界队列，不代表 CAN ACK 或传感器已处理。 */
bs_status_t drv_can_send(drv_can_id_t id, const drv_can_frame_t *frame,
                         uint32_t *request_id);
bs_status_t drv_can_read(drv_can_id_t id, drv_can_rx_t *rx);
bs_status_t drv_can_take_event(drv_can_id_t id, drv_can_event_t *event);
bs_status_t drv_can_get_status(drv_can_id_t id, drv_can_status_t *status);
/* 仅请求恢复；Bus-off 的实际恢复策略和条件由监督策略决定。 */
bs_status_t drv_can_request_recovery(drv_can_id_t id);
#endif

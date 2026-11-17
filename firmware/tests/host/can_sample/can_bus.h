#ifndef CAN_BUS_H
#define CAN_BUS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    CAN_BUS_OK = 0,
    CAN_BUS_EMPTY,
    CAN_BUS_BUSY,
    CAN_BUS_INVALID_ARGUMENT,
    CAN_BUS_NOT_READY,
    CAN_BUS_BUS_OFF,
    CAN_BUS_DRIVER_ERROR
} can_bus_result_t;

typedef enum
{
    CAN_BUS_STATE_UNINITIALIZED = 0,
    CAN_BUS_STATE_READY,
    CAN_BUS_STATE_BUS_OFF,
    CAN_BUS_STATE_FAULT
} can_bus_state_t;

/* 首轮仅定义经典 CAN 数据帧。CAN FD/远程帧按实际设备协议扩展。 */
typedef struct
{
    uint32_t id;
    uint32_t received_at_ms; /* 仅接收路径有效：MCU 接收时间，非传感器采样时间 */
    uint8_t data[8];
    uint8_t length;
    bool is_extended;
} can_frame_t;

typedef struct
{
    uint32_t rx_received;
    uint32_t rx_dropped;
    uint32_t rx_invalid;
    uint32_t tx_completed;
    uint32_t tx_failed;
    uint32_t bus_off_count;
} can_bus_stats_t;

/* 以下 API 仅由主循环调用，不可从中断上下文调用。 */
can_bus_result_t can_bus_init(void);
can_bus_result_t can_bus_send(const can_frame_t *frame);
can_bus_result_t can_bus_read(can_frame_t *frame);
can_bus_result_t can_bus_recover(void);
can_bus_result_t can_bus_get_state(can_bus_state_t *state);
can_bus_result_t can_bus_get_stats(can_bus_stats_t *stats);

#endif

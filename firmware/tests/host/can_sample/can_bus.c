#include "can_bus.h"
#include "can_bus_board.h"
#include "can_bus_internal.h"

#include <stddef.h>
#include <stdint.h>

/* 实际最多缓存 8 帧；额外 1 格用于区分队列满/空。 */
#define CAN_RX_STORAGE_COUNT  (9U)

static can_frame_t rx_frames[CAN_RX_STORAGE_COUNT];
static volatile uint8_t rx_head;
static volatile uint8_t rx_tail;
static volatile can_bus_state_t bus_state = CAN_BUS_STATE_UNINITIALIZED;
static can_bus_stats_t bus_stats;
static volatile uint32_t bus_off_event_sequence;

static bool frame_is_valid(const can_frame_t *frame)
{
    if (frame == NULL)
    {
        return false;
    }

    if (frame->length > 8U)
    {
        return false;
    }

    if (frame->is_extended)
    {
        return frame->id <= UINT32_C(0x1FFFFFFF);
    }

    return frame->id <= UINT32_C(0x7FF);
}

static uint8_t next_rx_index(uint8_t index)
{
    index++;
    if (index >= CAN_RX_STORAGE_COUNT)
    {
        index = 0U;
    }
    return index;
}

static void increase_saturated(uint32_t *value)
{
    if (*value < UINT32_MAX)
    {
        (*value)++;
    }
}

/* 从 CAN ISR 或已经进入临界区的主循环调用。 */
static void record_bus_off(void)
{
    bus_off_event_sequence++;
    if (bus_state != CAN_BUS_STATE_BUS_OFF)
    {
        increase_saturated(&bus_stats.bus_off_count);
    }

    /* 异常前的已缓存数据不允许在恢复后继续作为新数据消费。 */
    rx_tail = rx_head;
    bus_state = CAN_BUS_STATE_BUS_OFF;
}

can_bus_result_t can_bus_init(void)
{
    can_board_result_t result;

    if (bus_state != CAN_BUS_STATE_UNINITIALIZED)
    {
        return CAN_BUS_NOT_READY;
    }

    rx_head = 0U;
    rx_tail = 0U;
    bus_stats = (can_bus_stats_t){0};
    bus_off_event_sequence = 0U;

    /* 板级初始化配置时钟、引脚、控制器、滤波器和收发器。
     * can_board_start() 前不得触发接收回调。
     */
    result = can_board_init();
    if (result != CAN_BOARD_OK)
    {
        bus_state = CAN_BUS_STATE_FAULT;
        return CAN_BUS_DRIVER_ERROR;
    }

    bus_state = CAN_BUS_STATE_READY;
    result = can_board_start();
    if (result != CAN_BOARD_OK)
    {
        bus_state = CAN_BUS_STATE_FAULT;
        return CAN_BUS_DRIVER_ERROR;
    }

    return CAN_BUS_OK;
}

can_bus_result_t can_bus_send(const can_frame_t *frame)
{
    can_board_result_t result;

    if (!frame_is_valid(frame))
    {
        return CAN_BUS_INVALID_ARGUMENT;
    }

    if (bus_state == CAN_BUS_STATE_BUS_OFF)
    {
        return CAN_BUS_BUS_OFF;
    }

    if (bus_state != CAN_BUS_STATE_READY)
    {
        return CAN_BUS_NOT_READY;
    }

    /* OK 只说明硬件已接受发送请求，不说明帧已发送成功。 */
    result = can_board_send(frame);
    if (result == CAN_BOARD_OK)
    {
        return CAN_BUS_OK;
    }
    if (result == CAN_BOARD_BUSY)
    {
        return CAN_BUS_BUSY;
    }
    if (result == CAN_BOARD_BUS_OFF)
    {
        uint32_t previous_state = can_board_enter_critical();
        record_bus_off();
        can_board_exit_critical(previous_state);
        return CAN_BUS_BUS_OFF;
    }

    return CAN_BUS_DRIVER_ERROR;
}

can_bus_result_t can_bus_read(can_frame_t *frame)
{
    uint32_t previous_state;
    can_bus_result_t result = CAN_BUS_EMPTY;

    if (frame == NULL)
    {
        return CAN_BUS_INVALID_ARGUMENT;
    }

    previous_state = can_board_enter_critical();
    if (bus_state == CAN_BUS_STATE_BUS_OFF)
    {
        result = CAN_BUS_BUS_OFF;
    }
    else if (bus_state != CAN_BUS_STATE_READY)
    {
        result = CAN_BUS_NOT_READY;
    }
    else if (rx_head != rx_tail)
    {
        *frame = rx_frames[rx_tail];
        rx_tail = next_rx_index(rx_tail);
        result = CAN_BUS_OK;
    }
    can_board_exit_critical(previous_state);

    return result;
}

can_bus_result_t can_bus_recover(void)
{
    can_board_result_t result;
    uint32_t previous_state;
    uint32_t event_sequence;

    previous_state = can_board_enter_critical();
    if (bus_state != CAN_BUS_STATE_BUS_OFF)
    {
        can_board_exit_critical(previous_state);
        return CAN_BUS_NOT_READY;
    }
    event_sequence = bus_off_event_sequence;
    can_board_exit_critical(previous_state);

    /* 板级函数必须无阻塞，并在恢复接收能力后才返回 OK。 */
    result = can_board_recover();
    if (result == CAN_BOARD_BUSY)
    {
        return CAN_BUS_BUSY;
    }
    if (result != CAN_BOARD_OK)
    {
        return CAN_BUS_DRIVER_ERROR;
    }

    previous_state = can_board_enter_critical();
    if (bus_off_event_sequence != event_sequence)
    {
        can_board_exit_critical(previous_state);
        return CAN_BUS_BUS_OFF;
    }
    if (bus_state == CAN_BUS_STATE_BUS_OFF)
    {
        bus_state = CAN_BUS_STATE_READY;
    }
    can_board_exit_critical(previous_state);
    return CAN_BUS_OK;
}

can_bus_result_t can_bus_get_state(can_bus_state_t *state)
{
    uint32_t previous_state;
    if (state == NULL)
    {
        return CAN_BUS_INVALID_ARGUMENT;
    }
    previous_state = can_board_enter_critical();
    *state = bus_state;
    can_board_exit_critical(previous_state);
    return CAN_BUS_OK;
}

can_bus_result_t can_bus_get_stats(can_bus_stats_t *stats)
{
    uint32_t previous_state;
    if (stats == NULL)
    {
        return CAN_BUS_INVALID_ARGUMENT;
    }
    previous_state = can_board_enter_critical();
    *stats = bus_stats;
    can_board_exit_critical(previous_state);
    return CAN_BUS_OK;
}

void can_bus_on_rx_isr(const can_frame_t *frame)
{
    uint8_t next;
    uint8_t i;
    can_frame_t copy = {0};

    if (bus_state != CAN_BUS_STATE_READY)
    {
        return;
    }
    if (!frame_is_valid(frame))
    {
        increase_saturated(&bus_stats.rx_invalid);
        return;
    }

    next = next_rx_index(rx_head);
    if (next == rx_tail)
    {
        increase_saturated(&bus_stats.rx_dropped);
        return;
    }

    copy.id = frame->id;
    copy.received_at_ms = frame->received_at_ms;
    copy.length = frame->length;
    copy.is_extended = frame->is_extended;
    for (i = 0U; i < copy.length; ++i)
    {
        copy.data[i] = frame->data[i];
    }
    rx_frames[rx_head] = copy;
    rx_head = next;
    increase_saturated(&bus_stats.rx_received);
}

void can_bus_on_tx_done_isr(bool successful)
{
    if (successful)
    {
        increase_saturated(&bus_stats.tx_completed);
    }
    else
    {
        increase_saturated(&bus_stats.tx_failed);
    }
}

void can_bus_on_bus_off_isr(void)
{
    record_bus_off();
}

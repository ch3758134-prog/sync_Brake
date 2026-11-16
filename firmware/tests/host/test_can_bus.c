#include "can_bus.h"
#include "can_bus_board.h"
#include "can_bus_internal.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static can_board_result_t recovery_result = CAN_BOARD_BUSY;
static unsigned int sent_count;

/* Host 测试替身仅限本测试目标，禁止被 Target 链接。 */
can_board_result_t can_board_init(void) { return CAN_BOARD_OK; }
can_board_result_t can_board_start(void) { return CAN_BOARD_OK; }
can_board_result_t can_board_send(const can_frame_t *frame)
{
    (void)frame;
    ++sent_count;
    return CAN_BOARD_OK;
}
can_board_result_t can_board_recover(void) { return recovery_result; }
uint32_t can_board_enter_critical(void) { return 0U; }
void can_board_exit_critical(uint32_t previous_state) { (void)previous_state; }

int main(void)
{
    can_frame_t frame = {0};
    can_frame_t received = {0};
    can_bus_stats_t stats = {0};
    can_bus_state_t state;
    uint8_t i;

    assert(can_bus_send(NULL) == CAN_BUS_INVALID_ARGUMENT);
    assert(can_bus_read(NULL) == CAN_BUS_INVALID_ARGUMENT);
    assert(can_bus_read(&received) == CAN_BUS_NOT_READY);
    assert(can_bus_init() == CAN_BUS_OK);
    assert(can_bus_init() == CAN_BUS_NOT_READY);

    frame.id = UINT32_C(0x123);
    frame.length = 2U;
    frame.data[0] = 0xA5U;
    frame.data[1] = 0x5AU;
    frame.received_at_ms = 1234U;
    assert(can_bus_send(&frame) == CAN_BUS_OK);
    assert(sent_count == 1U);
    can_bus_on_rx_isr(&frame);
    assert(can_bus_read(&received) == CAN_BUS_OK);
    assert(received.id == frame.id);
    assert(received.data[0] == 0xA5U);
    assert(received.data[1] == 0x5AU);
    assert(received.data[2] == 0U);
    assert(received.received_at_ms == 1234U);
    assert(can_bus_read(&received) == CAN_BUS_EMPTY);
    assert(can_bus_get_state(NULL) == CAN_BUS_INVALID_ARGUMENT);
    assert(can_bus_get_stats(NULL) == CAN_BUS_INVALID_ARGUMENT);

    frame.id = UINT32_C(0x800);
    assert(can_bus_send(&frame) == CAN_BUS_INVALID_ARGUMENT);
    frame.is_extended = true;
    assert(can_bus_send(&frame) == CAN_BUS_OK);
    frame.id = UINT32_C(0x20000000);
    assert(can_bus_send(&frame) == CAN_BUS_INVALID_ARGUMENT);
    frame.id = UINT32_C(0x123);
    frame.length = 9U;
    assert(can_bus_send(&frame) == CAN_BUS_INVALID_ARGUMENT);
    frame.length = 2U;
    frame.is_extended = false;
    can_bus_on_rx_isr(NULL);
    assert(can_bus_get_stats(&stats) == CAN_BUS_OK);
    assert(stats.rx_invalid == 1U);

    for (i = 0U; i < 9U; ++i)
    {
        can_bus_on_rx_isr(&frame);
    }
    assert(can_bus_get_stats(&stats) == CAN_BUS_OK);
    assert(stats.rx_dropped == 1U);
    assert(stats.rx_received == 9U);

    can_bus_on_bus_off_isr();
    assert(can_bus_get_state(&state) == CAN_BUS_OK);
    assert(state == CAN_BUS_STATE_BUS_OFF);
    assert(can_bus_send(&frame) == CAN_BUS_BUS_OFF);
    assert(can_bus_read(&received) == CAN_BUS_BUS_OFF);
    assert(can_bus_recover() == CAN_BUS_BUSY);
    recovery_result = CAN_BOARD_OK;
    assert(can_bus_recover() == CAN_BUS_OK);
    assert(can_bus_read(&received) == CAN_BUS_EMPTY);
    assert(can_bus_get_stats(&stats) == CAN_BUS_OK);
    assert(stats.bus_off_count == 1U);
    assert(can_bus_recover() == CAN_BUS_NOT_READY);

    can_bus_on_tx_done_isr(true);
    can_bus_on_tx_done_isr(false);
    assert(can_bus_get_stats(&stats) == CAN_BUS_OK);
    assert(stats.tx_completed == 1U);
    assert(stats.tx_failed == 1U);

    puts("CAN bus Host behavior tests: PASS");
    return 0;
}

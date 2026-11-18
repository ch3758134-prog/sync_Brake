#include "drv_can.h"
#include "drv_gpio.h"
#include "drv_timer.h"
#include "drv_uart.h"
#include "drv_i2c.h"
#include "drv_spi.h"
#include "drv_clock.h"
#include "drv_reset.h"
#include "drv_adc.h"
#include "imu_can.h"
#include "bma510.h"
#include "ble_serial.h"
#include <assert.h>
#include <stddef.h>

int main(void)
{
    drv_can_frame_t frame = {0};
    assert(drv_can_frame_validate(NULL) == BS_STATUS_INVALID_ARGUMENT);
    assert(drv_can_frame_validate(&frame) == BS_STATUS_OK);
    frame.arbitration_id = 0x800U;
    assert(drv_can_frame_validate(&frame) == BS_STATUS_DATA_INVALID);
    frame.id_format = DRV_CAN_ID_EXTENDED;
    assert(drv_can_frame_validate(&frame) == BS_STATUS_OK);
    frame.arbitration_id = 0x20000000U;
    assert(drv_can_frame_validate(&frame) == BS_STATUS_DATA_INVALID);
    frame.arbitration_id = 0x1FFFFFFFU;
    frame.length = 9U;
    assert(drv_can_frame_validate(&frame) == BS_STATUS_DATA_INVALID);
    frame.length = 8U;
    assert(drv_can_frame_validate(&frame) == BS_STATUS_OK);
    frame.id_format = (drv_can_id_format_t)99;
    assert(drv_can_frame_validate(&frame) == BS_STATUS_DATA_INVALID);
    return 0;
}

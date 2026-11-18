#include "drv_can.h"

#include <stddef.h>
#include <stdint.h>

/* 只检查 Classic CAN 数据帧，既不访问 CAN 控制器也不发送报文。 */
bs_status_t drv_can_frame_validate(const drv_can_frame_t *frame)
{
    if (frame == NULL) {
        return BS_STATUS_INVALID_ARGUMENT;
    }
    if (frame->length > 8U) {
        return BS_STATUS_DATA_INVALID;
    }
    if (frame->id_format == DRV_CAN_ID_STANDARD) {
        if (frame->arbitration_id > UINT32_C(0x7FF)) {
            return BS_STATUS_DATA_INVALID;
        }
    } else if (frame->id_format == DRV_CAN_ID_EXTENDED) {
        if (frame->arbitration_id > UINT32_C(0x1FFFFFFF)) {
            return BS_STATUS_DATA_INVALID;
        }
    } else {
        return BS_STATUS_DATA_INVALID;
    }
    return BS_STATUS_OK;
}

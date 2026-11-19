#ifndef IMU_CAN_H
#define IMU_CAN_H
#include <stdint.h>
#include "bs_status.h"
#include "cdd_motion.h"
typedef struct {
    bool connected;
    bool protocol_valid;
    uint32_t last_received_at_ms;
    uint32_t command_errors;
    uint32_t decode_errors;
} imu_can_status_t;
/* 具体 CAN ID、启动指令和周期报文只能来自已确认的设备协议配置。 */
bs_status_t imu_can_init(void);
bs_status_t imu_can_process(uint32_t now_ms);
bs_status_t imu_can_get_observation(cdd_motion_observation_t *obs);
bs_status_t imu_can_get_status(imu_can_status_t *status);
#endif

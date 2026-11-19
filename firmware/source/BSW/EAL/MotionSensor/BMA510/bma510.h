#ifndef BMA510_H
#define BMA510_H
#include <stdint.h>
#include "bs_status.h"
#include "cdd_motion.h"
/* 仅在真实使用 BMA510 芯片时实现；与外接 CAN IMU 不是同一设备。 */
bs_status_t bma510_init(void);
bs_status_t bma510_process(uint32_t now_ms);
bs_status_t bma510_get_observation(cdd_motion_observation_t *obs);
#endif

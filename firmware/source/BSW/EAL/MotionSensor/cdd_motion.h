#ifndef CDD_MOTION_H
#define CDD_MOTION_H
#include <stdbool.h>
#include <stdint.h>
#include "bs_observation.h"
typedef struct {
    int32_t accel_longitudinal_mm_s2; /* 完成设备量纲与坐标转换后有效 */
    bool mounting_verified;
    bs_observation_meta_t meta;
} cdd_motion_observation_t;
#endif

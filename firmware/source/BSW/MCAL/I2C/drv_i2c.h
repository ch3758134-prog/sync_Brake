#ifndef DRV_I2C_H
#define DRV_I2C_H
#include "drv_types.h"
typedef enum { DRV_I2C_WRITE = 0, DRV_I2C_READ } drv_i2c_direction_t;
typedef struct {
    uint16_t address;       /* 7/10-bit 模式必须通过 BoardCfg 能力确认 */
    drv_i2c_direction_t direction;
    uint8_t *buffer;
    uint16_t size;
    bool stop_after;       /* 重复起始/事务组合支持能力待具体实现确认 */
} drv_i2c_segment_t;
typedef struct {
    uint32_t request_id;
    bool complete;
    bs_status_t completion_status;
    uint16_t transferred_bytes;
} drv_i2c_result_t;
/* 事务全部 segment 及 buffer 在结束前必须保持有效且只由驱动/硬件访问。
 * 一次只允许一个活动事务；执行方须定义事务之间重复起始支持。 */
bs_status_t drv_i2c_init(drv_i2c_id_t id);
bs_status_t drv_i2c_submit(drv_i2c_id_t id,
                           const drv_i2c_segment_t *segments, uint8_t count,
                           uint32_t *request_id);
bs_status_t drv_i2c_get_result(drv_i2c_id_t id, drv_i2c_result_t *result);
bs_status_t drv_i2c_abort(drv_i2c_id_t id, uint32_t request_id);
#endif

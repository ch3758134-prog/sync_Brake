#ifndef BRAKESYNC_BMA510_DEVICE_H
#define BRAKESYNC_BMA510_DEVICE_H
#include <stdbool.h>
#include <stdint.h>
/* BST-BMA510-DS000-03，I2C default 7bit 地址 0x18。
 * SPI 读取有 dummy byte，SPI 适配不得直接套用 I2C 传输格式。 */
#define BMA510_I2C_ADDRESS (0x18U)
typedef bool (*bma510_reg_read_t)(void *context, uint8_t address,
                                  uint8_t *dst, uint16_t size);
typedef bool (*bma510_reg_write_t)(void *context, uint8_t address,
                                   const uint8_t *src, uint16_t size);
typedef void (*bma510_delay_ms_t)(void *context, uint32_t ms);
typedef struct {
    bma510_reg_read_t read;
    bma510_reg_write_t write;
    bma510_delay_ms_t delay_ms;
    void *context;
} bma510_bus_t;
typedef enum {
    BMA510_OK = 0,
    BMA510_NOT_READY,
    BMA510_INVALID_ARGUMENT,
    BMA510_BUS_ERROR,
    BMA510_CHIP_ID_ERROR,
    BMA510_HEALTH_ERROR,
    BMA510_SAMPLE_INVALID
} bma510_result_t;
typedef struct {
    int16_t raw_x;
    int16_t raw_y;
    int16_t raw_z;
    int32_t x_mm_s2;
    int32_t y_mm_s2;
    int32_t z_mm_s2;
} bma510_sample_t;
typedef struct {
    bma510_bus_t bus;
    bool initialized;
} bma510_device_t;

/* 设备寄存器配置：100Hz、±2g、HPM，未设置输出方向/车身安装方向。 */
bma510_result_t BMA510_DeviceInit(bma510_device_t *device,
                                   const bma510_bus_t *bus);
/* 未到数据就绪时返回 NOT_READY；不重用旧样本，不把异常原始值输出为有效加速度。 */
bma510_result_t BMA510_DeviceRead(bma510_device_t *device,
                                   bma510_sample_t *sample);
#endif
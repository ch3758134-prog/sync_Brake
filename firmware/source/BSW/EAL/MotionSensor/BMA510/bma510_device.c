#include "bma510_device.h"
#include <stddef.h>
#include <string.h>

#define REG_CHIP_ID   (0x00U)
#define REG_HEALTH    (0x02U)
#define REG_STATUS    (0x11U)
#define REG_ACC_DATA  (0x18U)
#define REG_ACC_CTRL  (0x30U)
#define REG_ACC_CONF1 (0x31U)
#define REG_ACC_CONF2 (0x32U)
#define REG_INT2      (0x35U)
#define CHIP_ID_VALUE (0xC0U)

static bool Read(bma510_device_t *d, uint8_t reg, uint8_t *dst, uint16_t n)
{
    return d->bus.read(d->bus.context, reg, dst, n);
}
static bool Write(bma510_device_t *d, uint8_t reg, uint8_t val)
{
    return d->bus.write(d->bus.context, reg, &val, 1U);
}

bma510_result_t BMA510_DeviceInit(bma510_device_t *d, const bma510_bus_t *bus)
{
    uint8_t value = 0U;
    if (d == NULL || bus == NULL || bus->read == NULL ||
        bus->write == NULL || bus->delay_ms == NULL) {
        return BMA510_INVALID_ARGUMENT;
    }
    memset(d, 0, sizeof(*d));
    d->bus = *bus;
    /* POR 3ms：第一次读仅选择通信协议；I2C 首次 NACK 属器件定义。 */
    d->bus.delay_ms(d->bus.context, 3U);
    (void)Read(d, REG_CHIP_ID, &value, 1U);
    if (!Read(d, REG_CHIP_ID, &value, 1U)) { return BMA510_BUS_ERROR; }
    if (value != CHIP_ID_VALUE) { return BMA510_CHIP_ID_ERROR; }
    if (!Read(d, REG_HEALTH, &value, 1U)) { return BMA510_BUS_ERROR; }
    if ((value & 0x0FU) != 0x0FU) { return BMA510_HEALTH_ERROR; }
    /* I2C 模式按 Bosch 指南将 INT2 设为输出；不代表已连接 MCU 中断脚。 */
    if (!Write(d, REG_INT2, 0x01U)) { return BMA510_BUS_ERROR; }
    if (!Write(d, REG_ACC_CTRL, 0x00U) ||
        !Write(d, REG_ACC_CONF1, 0xA6U) ||
        !Write(d, REG_ACC_CONF2, 0x0CU) ||
        !Write(d, REG_ACC_CTRL, 0x0FU)) {
        return BMA510_BUS_ERROR;
    }
    d->initialized = true;
    return BMA510_OK;
}

static int16_t ToSigned16(uint8_t lo, uint8_t hi)
{
    uint16_t u = (uint16_t)lo | ((uint16_t)hi << 8);
    return (u <= INT16_MAX) ? (int16_t)u : (int16_t)((int32_t)u - 65536);
}
static int32_t ToMillimetersPerSecondSquared(int16_t raw)
{
    /* 2g: 16384 LSB/g; g=9806.65mm/s², 64位避免中间溢出。 */
    return (int32_t)(((int64_t)raw * 980665LL) / 1638400LL);
}

bma510_result_t BMA510_DeviceRead(bma510_device_t *d, bma510_sample_t *sample)
{
    uint8_t status;
    uint8_t bytes[6];
    int16_t x, y, z;
    if (d == NULL || sample == NULL) { return BMA510_INVALID_ARGUMENT; }
    if (!d->initialized) { return BMA510_NOT_READY; }
    if (!Read(d, REG_STATUS, &status, 1U)) { return BMA510_BUS_ERROR; }
    if ((status & 0x01U) == 0U) { return BMA510_NOT_READY; }
    /* Bosch 要求 X/Y/Z 必须在同一个 6字节 burst 中读取。 */
    if (!Read(d, REG_ACC_DATA, bytes, 6U)) { return BMA510_BUS_ERROR; }
    x = ToSigned16(bytes[0], bytes[1]);
    y = ToSigned16(bytes[2], bytes[3]);
    z = ToSigned16(bytes[4], bytes[5]);
    if (x == INT16_MIN || y == INT16_MIN || z == INT16_MIN) {
        return BMA510_SAMPLE_INVALID;
    }
    sample->raw_x = x; sample->raw_y = y; sample->raw_z = z;
    sample->x_mm_s2 = ToMillimetersPerSecondSquared(x);
    sample->y_mm_s2 = ToMillimetersPerSecondSquared(y);
    sample->z_mm_s2 = ToMillimetersPerSecondSquared(z);
    return BMA510_OK;
}
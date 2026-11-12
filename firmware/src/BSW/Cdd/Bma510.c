#include "Bma510.h"
#include <stddef.h>
#define ADDR7 0x18u
#define CHIP_ID 0x00u
#define HEALTH 0x02u
#define CONFIG 0x10u
#define STATUS 0x11u
#define XYZ 0x18u
#define ACC0 0x30u
#define ACC1 0x31u
#define ACC2 0x32u
#define INT2 0x35u
static Bma510Status Read(Bma510 *d, uint8_t reg, uint8_t *data, size_t n)
{ return d->bus.read(d->bus.context, ADDR7, reg, data, n) == 0 ? BMA510_OK : BMA510_E_IO; }
static Bma510Status Write(Bma510 *d, uint8_t reg, uint8_t val)
{ return d->bus.write(d->bus.context, ADDR7, reg, &val, 1u) == 0 ? BMA510_OK : BMA510_E_IO; }
static int16_t Signed16(uint8_t lo, uint8_t hi)
{
    uint16_t value = (uint16_t)((uint16_t)lo | ((uint16_t)hi << 8));
    int32_t wide = value < 0x8000u ? (int32_t)value : (int32_t)value - 65536;
    return (int16_t)wide;
}
Bma510Status Bma510_Init(Bma510 *d)
{
    uint8_t v = 0;
    Bma510Status s;
    if (!d || !d->bus.read || !d->bus.write || !d->bus.delay_ms) return BMA510_E_ARG;
    d->initialized = 0u;
    d->bus.delay_ms(d->bus.context, 3u);
    /* I2C 首事务用于接口选择，首次 NACK 不视为故障。 */
    (void)Read(d, CHIP_ID, &v, 1u);
    if ((s = Read(d, CHIP_ID, &v, 1u)) != BMA510_OK) return s;
    if (v != 0xc0u) return BMA510_E_CHIP;
    if ((s = Read(d, HEALTH, &v, 1u)) != BMA510_OK) return s;
    if ((v & 0x0fu) != 0x0fu) return BMA510_E_HEALTH;
    if ((s = Write(d, INT2, 0x01u)) != BMA510_OK) return s;
    if ((s = Write(d, ACC0, 0x00u)) != BMA510_OK) return s;
    if ((s = Write(d, ACC1, 0xa6u)) != BMA510_OK) return s;
    if ((s = Write(d, ACC2, 0x0cu)) != BMA510_OK) return s;
    if ((s = Write(d, ACC0, 0x0fu)) != BMA510_OK) return s;
    if ((s = Read(d, CONFIG, &v, 1u)) != BMA510_OK) return s;
    if ((v & 0x02u) != 0u) return BMA510_E_CONFIG;
    d->initialized = 1u;
    return BMA510_OK;
}
Bma510Status Bma510_Read(Bma510 *d, Bma510Sample *out)
{
    uint8_t status=0u, raw[6]={0u};
    Bma510Status s;
    if (!d || !out || !d->bus.read || !d->bus.write || !d->initialized) return BMA510_E_ARG;
    if ((s=Read(d, STATUS, &status, 1u)) != BMA510_OK) return s;
    if ((status & 1u)==0u) return BMA510_NO_DATA;
    if ((s=Read(d, XYZ, raw, 6u)) != BMA510_OK) return s;
    /* SENSOR_STATUS bit0 使用 W1C 语义清除。 */
    if ((s=Write(d, STATUS, 1u)) != BMA510_OK) return s;
    if ((raw[0]==0 && raw[1]==0x80u) || (raw[2]==0 && raw[3]==0x80u)
        || (raw[4]==0 && raw[5]==0x80u)) return BMA510_E_INVALID;
    out->x=Signed16(raw[0],raw[1]);
    out->y=Signed16(raw[2],raw[3]);
    out->z=Signed16(raw[4],raw[5]);
    return BMA510_OK;
}
int32_t Bma510_ToMilliG(int16_t raw) { return ((int32_t)raw*2000)/32768; }

#include "bma510_device.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    uint8_t reg[256];
    unsigned calls;
    unsigned delay;
    bool first_nack;
    bool fail_bus;
} fake_t;
static bool fake_read(void *context, uint8_t r, uint8_t *p, uint16_t n)
{
    fake_t *f = context;
    ++f->calls;
    if (f->first_nack) { f->first_nack = false; return false; }
    if (f->fail_bus) { return false; }
    memcpy(p, &f->reg[r], n);
    return true;
}
static bool fake_write(void *context, uint8_t r, const uint8_t *p, uint16_t n)
{
    fake_t *f = context;
    if (f->fail_bus) { return false; }
    memcpy(&f->reg[r], p, n);
    return true;
}
static void fake_delay(void *context, uint32_t n)
{
    ((fake_t *)context)->delay += n;
}
int main(void)
{
    fake_t f = {0};
    bma510_device_t dev;
    bma510_sample_t s = {0};
    bma510_bus_t bus = {fake_read, fake_write, fake_delay, &f};
    f.reg[0] = 0xC0;
    f.reg[2] = 0x0F;
    f.first_nack = true;
    assert(BMA510_DeviceInit(&dev, &bus) == BMA510_OK);
    assert(f.delay == 3U && f.reg[0x30] == 0x0F &&
           f.reg[0x31] == 0xA6 && f.reg[0x32] == 0x0C);
    assert(BMA510_DeviceRead(&dev, &s) == BMA510_NOT_READY);
    f.reg[0x11] = 1U;
    /* X = +1g=16384, Y = -1g=-16384, Z=0 */
    f.reg[0x18] = 0x00; f.reg[0x19] = 0x40;
    f.reg[0x1A] = 0x00; f.reg[0x1B] = 0xC0;
    f.reg[0x1C] = 0x00; f.reg[0x1D] = 0x00;
    assert(BMA510_DeviceRead(&dev, &s) == BMA510_OK);
    assert(s.raw_x == 16384 && s.raw_y == -16384 && s.raw_z == 0);
    assert(s.x_mm_s2 == 9806 && s.y_mm_s2 == -9806);
    f.reg[0x19] = 0x80;
    assert(BMA510_DeviceRead(&dev, &s) == BMA510_SAMPLE_INVALID);
    f.fail_bus = true;
    assert(BMA510_DeviceRead(&dev, &s) == BMA510_BUS_ERROR);
    puts("BMA510 register + sample host tests PASS");
    return 0;
}
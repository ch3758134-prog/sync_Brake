#ifndef BRAKESYNC_BMA510_H
#define BRAKESYNC_BMA510_H
#include <stdint.h>
#include <stddef.h>
typedef enum {
    BMA510_OK = 0, BMA510_NO_DATA = 1, BMA510_E_ARG = -1,
    BMA510_E_IO = -2, BMA510_E_CHIP = -3, BMA510_E_HEALTH = -4,
    BMA510_E_CONFIG = -5, BMA510_E_INVALID = -6
} Bma510Status;
typedef struct {
    void *context;
    int (*read)(void *, uint8_t addr7, uint8_t reg, uint8_t *out, size_t len);
    int (*write)(void *, uint8_t addr7, uint8_t reg, const uint8_t *data, size_t len);
    void (*delay_ms)(void *, uint32_t);
} Bma510Bus;
typedef struct { int16_t x, y, z; } Bma510Sample;
typedef struct { Bma510Bus bus; uint8_t initialized; } Bma510;
Bma510Status Bma510_Init(Bma510 *dev);
Bma510Status Bma510_Read(Bma510 *dev, Bma510Sample *sample);
int32_t Bma510_ToMilliG(int16_t raw); /* 当前配置 ±2g */
#endif

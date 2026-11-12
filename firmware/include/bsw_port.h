#ifndef BRAKESYNC_BSW_PORT_H
#define BRAKESYNC_BSW_PORT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 官方 SDK 外设薄适配合同；禁止在此层出现制动业务概念。 */
typedef enum {
    BS_OK = 0, BS_E_ARGUMENT = -1, BS_E_UNBOUND = -2,
    BS_E_IO = -3, BS_E_TIMEOUT = -4, BS_E_BUSY = -5,
    BS_E_RANGE = -6, BS_E_UNSUPPORTED = -7
} BsStatus;

typedef struct {
    uint32_t id;          /* 标准 11bit 或扩展 29bit ID */
    uint8_t dlc;          /* 首期只支持 CAN classic DLC 0..8 */
    bool extended;
    bool remote;
    uint8_t bytes[8];
} BsCanFrame;

typedef struct {
    void *context;      /* 兼容原有共用 Board 上下文 */
    void *gpio_context; /* 单独设备上下文，非空时优先 */
    void *can_context;
    void *timer_context;
    void *uart_context;
    void *i2c_context;
    BsStatus (*gpio_read)(void *, uint16_t logical_pin, bool *level);
    BsStatus (*gpio_write)(void *, uint16_t logical_pin, bool level);
    BsStatus (*can_send)(void *, uint8_t bus, const BsCanFrame *);
    BsStatus (*can_receive)(void *, uint8_t bus, BsCanFrame *);
    BsStatus (*timer_now_ms)(void *, uint32_t *);
    BsStatus (*uart_read)(void *, uint8_t bus, uint8_t *, size_t capacity, size_t *received);
    BsStatus (*uart_write)(void *, uint8_t bus, const uint8_t *, size_t count, uint32_t timeout_ms);
    BsStatus (*i2c_reg_read)(void *, uint8_t bus, uint8_t addr7, uint8_t reg,
                            uint8_t *out, size_t count);
    BsStatus (*i2c_reg_write)(void *, uint8_t bus, uint8_t addr7, uint8_t reg,
                             const uint8_t *in, size_t count);
} BsPort;

/* 需在启动阶段由 Board 层绑定。未绑定时所有 API 明确报错。 */
BsStatus Bs_PortBind(const BsPort *port);
void Bs_PortUnbind(void);
BsStatus Bs_GpioRead(uint16_t pin, bool *level);
BsStatus Bs_GpioWrite(uint16_t pin, bool level);
BsStatus Bs_CanSend(uint8_t bus, const BsCanFrame *frame);
BsStatus Bs_CanReceive(uint8_t bus, BsCanFrame *frame);
BsStatus Bs_TimerNowMs(uint32_t *now);
BsStatus Bs_UartRead(uint8_t bus, uint8_t *out, size_t capacity, size_t *received);
BsStatus Bs_UartWrite(uint8_t bus, const uint8_t *data, size_t count, uint32_t timeout_ms);
BsStatus Bs_I2cRegRead(uint8_t bus, uint8_t addr7, uint8_t reg, uint8_t *out, size_t count);
BsStatus Bs_I2cRegWrite(uint8_t bus, uint8_t addr7, uint8_t reg, const uint8_t *data, size_t count);

#endif

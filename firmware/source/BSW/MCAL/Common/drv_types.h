#ifndef DRV_TYPES_H
#define DRV_TYPES_H
#include <stdbool.h>
#include <stdint.h>
#include "bs_status.h"
/* 都是逻辑资源 ID，不等于 MCU instance / Port / Pin / IRQ。 */
typedef uint16_t drv_gpio_id_t;
typedef uint8_t drv_timer_id_t;
typedef uint8_t drv_uart_id_t;
typedef uint8_t drv_can_id_t;
typedef uint8_t drv_i2c_id_t;
typedef uint8_t drv_spi_id_t;
#endif

#ifndef DRV_SPI_H
#define DRV_SPI_H
#include "drv_types.h"
typedef struct {
    const uint8_t *tx;
    uint8_t *rx;
    uint16_t bytes;
} drv_spi_transfer_t;
typedef struct {
    uint32_t request_id;
    bool complete;
    bs_status_t completion_status;
} drv_spi_result_t;
/* SPI mode、bit rate、CS 由 Board 配置；使用期间 TX/RX 保持有效。 */
bs_status_t drv_spi_init(drv_spi_id_t id);
bs_status_t drv_spi_submit(drv_spi_id_t id,
                           const drv_spi_transfer_t *transfer,
                           uint32_t *request_id);
bs_status_t drv_spi_get_result(drv_spi_id_t id, drv_spi_result_t *result);
bs_status_t drv_spi_abort(drv_spi_id_t id, uint32_t request_id);
#endif

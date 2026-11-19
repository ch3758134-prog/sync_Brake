#ifndef DRV_ADC_H
#define DRV_ADC_H
#include <stdbool.h>
#include <stdint.h>
#include "bs_status.h"
typedef uint16_t drv_adc_id_t; /* BoardCfg 逻辑 ADC 输入，不含芯片通道号 */
typedef struct {
    uint32_t raw_code;
    uint8_t resolution_bits;
    uint32_t observed_at_ms;
    bool calibration_valid;
} drv_adc_sample_t;
/* 仅原始 ADC 观测；真实电流/电压转换与诊断取决于未来电路。 */
bs_status_t drv_adc_read_sample(drv_adc_id_t id, drv_adc_sample_t *sample);
#endif

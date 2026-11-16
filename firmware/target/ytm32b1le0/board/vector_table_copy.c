/*
 * Copyright 2020-2026 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file vector_table_copy.c
 * @brief 
 * 
 */

#include <stdint.h>
#include "device_registers.h"

static volatile uint32_t * const s_vectors[NUMBER_OF_CORES] = FEATURE_INTERRUPT_INT_VECTORS;
extern uint32_t Image$$IVT_RAM_start$$Base;
extern uint32_t Image$$IVT_start$$Base;
extern uint32_t Image$$IVT_end$$Limit;



void VectorTableCopy(){
    uint8_t coreId = (uint8_t)GET_CORE_ID();
    uint32_t* ramStart = (uint32_t*)&Image$$IVT_RAM_start$$Base;
    const uint32_t* romStart = (const uint32_t*)&Image$$IVT_start$$Base;
    uint32_t size=((uint32_t)&Image$$IVT_end$$Limit-(uint32_t)&Image$$IVT_start$$Base)/sizeof(uint32_t);
    *s_vectors[coreId] = (uint32_t)&Image$$IVT_RAM_start$$Base;

    for (uint32_t n = 0; n < size; n++)
    {
        ramStart[n] = romStart[n];
    }
}


void DefaultISR(void)
{
#ifndef YTM32_DEVELOPMENT_MODE
   SystemSoftwareReset();
#endif /* YTM32_DEVELOPMENT_MODE */
   while(1){};
}

__attribute__ ((weak)) void NMI_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void HardFault_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void SVC_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void PendSV_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void SysTick_Handler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void DMA0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void DMA1_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void DMA2_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void DMA3_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void DMA_Error_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void EMU0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void RTC_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void SPI2_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void lpTMR0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void GPIO_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN0_ORed_Err_Wakeup_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN0_ORed_0_15_MB_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR0_CH_0_7_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR0_Fault_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR0_Ovf_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR1_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void eTMR2_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void CAN0_ORed_16_31_MB_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void EFM_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void pTMR0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void PCU_CMU_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void WDG0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void UART2_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void I2C0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void I2C1_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void SPI0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void SPI1_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void ADC0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void ACMP0_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void UART1_IRQHandler(void) {
    DefaultISR();
}
__attribute__ ((weak)) void UART0_IRQHandler(void) {
    DefaultISR();
}

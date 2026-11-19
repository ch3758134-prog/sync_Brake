/*
 * Copyright 2020-2026 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file vector.S
 * @brief 
 * 
 */

    PRESERVE8
    THUMB
    AREA    |.isr_vector|, CODE, READONLY
    IMPORT   Reset_Handler             /* Reset Handler */
    
    IMPORT   DefaultISR
    IMPORT  |Image$$STACK_end$$Limit|
    IMPORT   NMI_Handler
    IMPORT   HardFault_Handler
    IMPORT   SVC_Handler
    IMPORT   PendSV_Handler
    IMPORT   SysTick_Handler
    IMPORT   DMA0_IRQHandler
    IMPORT   DMA1_IRQHandler
    IMPORT   DMA2_IRQHandler
    IMPORT   DMA3_IRQHandler
    IMPORT   DMA_Error_IRQHandler
    IMPORT   EMU0_IRQHandler
    IMPORT   RTC_IRQHandler
    IMPORT   SPI2_IRQHandler
    IMPORT   lpTMR0_IRQHandler
    IMPORT   GPIO_IRQHandler
    IMPORT   CAN0_ORed_Err_Wakeup_IRQHandler
    IMPORT   CAN0_ORed_0_15_MB_IRQHandler
    IMPORT   eTMR0_CH_0_7_IRQHandler
    IMPORT   eTMR0_Fault_IRQHandler
    IMPORT   eTMR0_Ovf_IRQHandler
    IMPORT   eTMR1_IRQHandler
    IMPORT   eTMR2_IRQHandler
    IMPORT   CAN0_ORed_16_31_MB_IRQHandler
    IMPORT   EFM_IRQHandler
    IMPORT   pTMR0_IRQHandler
    IMPORT   PCU_CMU_IRQHandler
    IMPORT   WDG0_IRQHandler
    IMPORT   UART2_IRQHandler
    IMPORT   I2C0_IRQHandler
    IMPORT   I2C1_IRQHandler
    IMPORT   SPI0_IRQHandler
    IMPORT   SPI1_IRQHandler
    IMPORT   ADC0_IRQHandler
    IMPORT   ACMP0_IRQHandler
    IMPORT   UART1_IRQHandler
    IMPORT   UART0_IRQHandler
    DCD |Image$$STACK_end$$Limit|   /* Top of Stack for Initial Stack Pointer */
    DCD Reset_Handler             /* Reset Handler */
    DCD NMI_Handler
    DCD HardFault_Handler
    DCD DefaultISR
    DCD DefaultISR
    DCD DefaultISR
    DCD DefaultISR
    DCD DefaultISR
    DCD DefaultISR
    DCD DefaultISR
    DCD SVC_Handler
    DCD DefaultISR
    DCD DefaultISR
    DCD PendSV_Handler
    DCD SysTick_Handler
    DCD DMA0_IRQHandler
    DCD DMA1_IRQHandler
    DCD DMA2_IRQHandler
    DCD DMA3_IRQHandler
    DCD DMA_Error_IRQHandler
    DCD EMU0_IRQHandler
    DCD RTC_IRQHandler
    DCD SPI2_IRQHandler
    DCD lpTMR0_IRQHandler
    DCD GPIO_IRQHandler
    DCD CAN0_ORed_Err_Wakeup_IRQHandler
    DCD CAN0_ORed_0_15_MB_IRQHandler
    DCD eTMR0_CH_0_7_IRQHandler
    DCD eTMR0_Fault_IRQHandler
    DCD eTMR0_Ovf_IRQHandler
    DCD eTMR1_IRQHandler
    DCD eTMR2_IRQHandler
    DCD CAN0_ORed_16_31_MB_IRQHandler
    DCD EFM_IRQHandler
    DCD DefaultISR
    DCD pTMR0_IRQHandler
    DCD PCU_CMU_IRQHandler
    DCD WDG0_IRQHandler
    DCD UART2_IRQHandler
    DCD I2C0_IRQHandler
    DCD I2C1_IRQHandler
    DCD SPI0_IRQHandler
    DCD SPI1_IRQHandler
    DCD ADC0_IRQHandler
    DCD ACMP0_IRQHandler
    DCD UART1_IRQHandler
    DCD UART0_IRQHandler
    END
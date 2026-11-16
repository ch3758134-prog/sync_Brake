/*
 * Copyright (c) 2021-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file system_YTM32B1LE0.c
 * @version 1.4.1
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 1533 Rule 8.9: The object '%1s' is only referenced by function '%2s'. 
 *
 */

#include "device_registers.h"
#include "system_YTM32B1LE0.h"
#include "stdbool.h"

/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK; /*PRQA S 1533*/

/*FUNCTION**********************************************************************
 *
 * Function Name : SystemInit
 * Description   : This function disables the watchdog, enables FPU
 * and the power mode protection if the corresponding feature macro
 * is enabled. SystemInit is called from startup_device file.
 *
 * Implements    : SystemInit_Activity
 *END**************************************************************************/
void SystemInit(void)
{
/**************************************************************************/
/* RCU CONTROL */
/**************************************************************************/
    IPC->CTRL[IPC_CIM_INDEX] |= IPC_CTRL_CLKEN_MASK;
    CIM->CTRL |= CIM_CTRL_LOCKUPEN_MASK;
/**************************************************************************/
/* WDOG DISABLE*/
/**************************************************************************/
#if (DISABLE_WDOG)
    WDG0->SVCR = 0xB631;
    WDG0->SVCR = 0xC278;
    WDG0->CR &= ~WDG_CR_EN_MASK;
#endif /* (DISABLE_WDOG) */
    /* Enable flash prefetch */
    EFM->CTRL |= EFM_CTRL_PREFETCH_EN_MASK;
#if defined(FEATURE_EFM_HAS_READONLY_BIT) && (FEATURE_EFM_HAS_READONLY_BIT == 1)
    /* Read only flash array */
    EFM->CTRL |= EFM_CTRL_READONLY_MASK;
#endif
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SystemCoreClockUpdate
 * Description   : This function must be called whenever the core clock is changed
 * during program execution. It evaluates the clock register settings and calculates
 * the current core clock.
 *
 * Implements    : SystemCoreClockUpdate_Activity
 *END**************************************************************************/
void SystemCoreClockUpdate(void)
{
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SystemSoftwareReset
 * Description   : This function is used to initiate a system reset
 *
 * Implements    : SystemSoftwareReset_Activity
 *END**************************************************************************/
void SystemSoftwareReset(void)
{
    NVIC_SystemReset();
}

/*FUNCTION**********************************************************************
 *
 * Function Name : SystemGetUid
 * Description   : This function is only used to get chip unit ID from reserved flash area.
 *
 * Implements    : SystemGetUid_Activity
 *END**************************************************************************/
void SystemGetUid(uint32_t *uidh, uint32_t *uidl)
{
    /* UID store in FLASH */
    *uidh = ((((uint32_t)(*(uint8_t*)0x10000FE0) & 0xFU) <<  0U)
           | (((uint32_t)(*(uint8_t*)0x10000FE1) & 0xFU) <<  4U)
           | (((uint32_t)(*(uint8_t*)0x10000FE2) & 0xFU) <<  8U)
           | (((uint32_t)(*(uint8_t*)0x10000FE3) & 0xFU) << 12U)
           | (((uint32_t)(*(uint8_t*)0x10000FE4) & 0xFU) << 16U)
           | (((uint32_t)(*(uint8_t*)0x10000FE5) & 0xFU) << 20U));
    *uidl = ((((uint32_t)(*(uint8_t*)0x10000FED) & 0xFFU) <<  0U)
           | (((uint32_t)(*(uint8_t*)0x10000FEC) & 0xFFU) <<  8U)
           | (((uint32_t)(*(uint8_t*)0x10000FE8) & 0xFFU) << 16U)
           | (((uint32_t)(*(uint8_t*)0x10000F40) & 0xFFU) << 24U));
}

/*******************************************************************************
 * EOF
 ******************************************************************************/

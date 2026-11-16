/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file interrupt_manager.c
 * @version 1.5.0
 *
 * @brief Interrupt Manager - implementation of the public INT_SYS_* API.
 *
 * This file implements the core interrupt-control helpers declared in
 * interrupt_manager.h. The routines wrap CMSIS NVIC services for handler
 * installation, IRQ enable or disable control, priority updates, nested
 * global interrupt masking, and optional pending or active-state queries.
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 *  PRQA S 0305, Rule 11.1: Required when converting between ISR pointers and
 *   the integral vector-table entries stored through `SCB->VTOR`.
 */

#include "interrupt_manager.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*!
 * @brief Nested global-disable counter used by the critical-section helpers.
 */
static int32_t g_interruptDisableCount = 0;

/*!
 * @brief PRIMASK value captured when the outermost critical section is entered.
 */
static uint32_t g_interruptPrimask = 0U;

/*******************************************************************************
 * Handler Installation
 ******************************************************************************/

/*!
 * @brief Install a handler into the active vector table.
 */
void INT_SYS_InstallHandler(IRQn_Type irqNumber, isr_t newHandler, isr_t *oldHandler)
{
#if (defined(DEV_ERROR_DETECT) || defined(CUSTOM_DEVASSERT))

    /* Use a signed copy so the max-range assert matches the enum domain. */
    int32_t dev_irqNumber = (int32_t)irqNumber;
    DEV_ASSERT(FEATURE_INTERRUPT_IRQ_MIN <= irqNumber);
    DEV_ASSERT(dev_irqNumber <= (int32_t)FEATURE_INTERRUPT_IRQ_MAX);
#ifdef __NO_VECTOR_TABLE_COPY
    DEV_ASSERT(false);
#endif
#endif /* (defined(DEV_ERROR_DETECT) || defined(CUSTOM_DEVASSERT)) */

    uint32_t *pVectorRam = (uint32_t *)(SCB->VTOR);

    /* External IRQ 0 starts at vector-table entry 16. */
    if (oldHandler != (isr_t *)0)
    {
        /* Return the previous vector entry when the caller requests it. */
        *oldHandler = (isr_t)pVectorRam[((int32_t)irqNumber) + 16]; /* PRQA S 0305 */
    }

    /* Replace the vector-table entry for the selected IRQ. */
    pVectorRam[((int32_t)irqNumber) + 16] = (uint32_t)newHandler; /* PRQA S 0305 */

#if defined(__DCACHE_PRESENT) && __DCACHE_PRESENT
    if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U)
    {
        /* Ensure the core fetches the updated handler entry. */
        SCB_CleanDCache();
        SCB_InvalidateICache();
    }
#endif
}

/*******************************************************************************
 * IRQ Line Control
 ******************************************************************************/

/*!
 * @brief Enable one IRQ line in the NVIC.
 */
void INT_SYS_EnableIRQ(IRQn_Type irqNumber)
{
    NVIC_EnableIRQ(irqNumber);
}

/*!
 * @brief Disable one IRQ line in the NVIC.
 */
void INT_SYS_DisableIRQ(IRQn_Type irqNumber)
{
    NVIC_DisableIRQ(irqNumber);
}

/*******************************************************************************
 * Global IRQ Control
 ******************************************************************************/

/*!
 * @brief Release one level of nested global interrupt masking.
 */
void INT_SYS_EnableIRQGlobal(void)
{
    /* Re-enable the core interrupt state only after all nesting levels exit. */
    if (g_interruptDisableCount > 0)
    {
        g_interruptDisableCount--;

        /* Only unmask if interrupts were enabled before the outermost entry. */
        if ((g_interruptDisableCount <= 0) && (g_interruptPrimask == 0U))
        {
            /* Restore global interrupt handling for the core. */
            __enable_irq();
        }
    }
}

/*!
 * @brief Disable global interrupts and enter a nested critical section.
 */
void INT_SYS_DisableIRQGlobal(void)
{
    /* Sample PRIMASK before masking so a caller-side __disable_irq() is preserved. */
    uint32_t primask = __get_PRIMASK();

    /* Mask all configurable interrupts at the core level immediately. */
    __disable_irq();

    /* Save the entry state only at the outermost nesting level. */
    if (g_interruptDisableCount == 0)
    {
        g_interruptPrimask = primask;
    }

    /* Track the nesting depth so the caller can balance critical sections. */
    g_interruptDisableCount++;
}

/*******************************************************************************
 * Priority Control
 ******************************************************************************/

/*!
 * @brief Program the priority of one IRQ or exception.
 */
void INT_SYS_SetPriority(IRQn_Type irqNumber, uint8_t priority)
{
    NVIC_SetPriority(irqNumber, priority);
}

/*!
 * @brief Read the programmed priority of one IRQ or exception.
 */
uint8_t INT_SYS_GetPriority(IRQn_Type irqNumber)
{
    return (uint8_t)NVIC_GetPriority(irqNumber);
}

#if FEATURE_INTERRUPT_HAS_PENDING_STATE

/*******************************************************************************
 * Pending State Control
 ******************************************************************************/

/*!
 * @brief Clear the pending state of one IRQ line.
 */
void INT_SYS_ClearPending(IRQn_Type irqNumber)
{
    NVIC_ClearPendingIRQ(irqNumber);
}

/*!
 * @brief Force one IRQ line into the pending state.
 */
void INT_SYS_SetPending(IRQn_Type irqNumber)
{
    NVIC_SetPendingIRQ(irqNumber);
}

/*!
 * @brief Read the pending state of one IRQ line.
 */
uint32_t INT_SYS_GetPending(IRQn_Type irqNumber)
{
    return NVIC_GetPendingIRQ(irqNumber);
}
#endif /* FEATURE_INTERRUPT_HAS_PENDING_STATE */

#if FEATURE_INTERRUPT_HAS_ACTIVE_STATE

/*******************************************************************************
 * Active State Query
 ******************************************************************************/

/*!
 * @brief Read the active state of one IRQ line.
 */
uint32_t INT_SYS_GetActive(IRQn_Type irqNumber)
{
    return NVIC_GetActive(irqNumber);
}
#endif /* FEATURE_INTERRUPT_HAS_ACTIVE_STATE */

/*******************************************************************************
 * EOF
 ******************************************************************************/

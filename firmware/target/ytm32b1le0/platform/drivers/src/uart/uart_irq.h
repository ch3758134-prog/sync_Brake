/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file uart_irq.h
 * @brief UART interrupt handler prototypes for each peripheral instance.
 *
 * This header declares the per-instance ISR entry points that are referenced
 * by the vector table (startup code).  Each handler dispatches directly to
 * the common UART_DRV_IRQHandler().
 */

#ifndef UART_IRQ_H__
#define UART_IRQ_H__

#include "device_registers.h"
#include "interrupt_manager.h"

/*******************************************************************************
 * Common IRQ Dispatcher
 ******************************************************************************/

/*!
 * @brief Common UART interrupt handler.
 *
 * Handles transmit, receive, and error interrupts for one UART instance.
 * Called from the per-instance ISR wrappers below.
 *
 * @param[in] instance  UART instance number (0-based).
 */
void UART_DRV_IRQHandler(uint32_t instance);

/*******************************************************************************
 * Per-Instance ISR Prototypes
 ******************************************************************************/

#if (UART_INSTANCE_COUNT > 0U)
/*! @brief UART0 interrupt handler (vector table entry). */
void UART0_IrqHandler(void);
#endif

#if (UART_INSTANCE_COUNT > 1U)
/*! @brief UART1 interrupt handler (vector table entry). */
void UART1_IrqHandler(void);
#endif

#if (UART_INSTANCE_COUNT > 2U)
/*! @brief UART2 interrupt handler (vector table entry). */
void UART2_IrqHandler(void);
#endif

#if (UART_INSTANCE_COUNT > 3U)
/*! @brief UART3 interrupt handler (vector table entry). */
void UART3_IrqHandler(void);
#endif

#if (UART_INSTANCE_COUNT > 4U)
/*! @brief UART4 interrupt handler (vector table entry). */
void UART4_IrqHandler(void);
#endif

#if (UART_INSTANCE_COUNT > 5U)
/*! @brief UART5 interrupt handler (vector table entry). */
void UART5_IrqHandler(void);
#endif

/*!
 * @brief ISR function-pointer table, indexed by UART instance number.
 *
 * Used by UART_DRV_Init() / UART_DRV_Deinit() to install / remove the
 * correct handler via the Interrupt Manager.
 */
extern isr_t g_uartIsr[UART_INSTANCE_COUNT];

#endif /* UART_IRQ_H__ */
/*******************************************************************************
 * EOF
 ******************************************************************************/

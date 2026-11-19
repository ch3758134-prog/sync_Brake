/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file uart_irq.c
 * @brief Per-instance UART ISR wrappers and ISR lookup table.
 *
 * Each UARTn_IrqHandler() is a thin wrapper that dispatches to the common
 * UART_DRV_IRQHandler(n).  The g_uartIsr[] table is used by the driver to
 * install / remove handlers at runtime via the Interrupt Manager.
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 1533 Rule 8.9: The object '%1s' is only referenced by function '%2s'.
 */

#include "uart_irq.h"

/*******************************************************************************
 * Per-Instance ISR Wrappers
 ******************************************************************************/

#if (UART_INSTANCE_COUNT > 0U)
/*! @brief UART0 ISR - dispatches to UART_DRV_IRQHandler(0). */
void UART0_IrqHandler(void)
{
    UART_DRV_IRQHandler(0);
}
#endif

#if (UART_INSTANCE_COUNT > 1U)
/*! @brief UART1 ISR - dispatches to UART_DRV_IRQHandler(1). */
void UART1_IrqHandler(void)
{
    UART_DRV_IRQHandler(1);
}
#endif

#if (UART_INSTANCE_COUNT > 2U)
/*! @brief UART2 ISR - dispatches to UART_DRV_IRQHandler(2). */
void UART2_IrqHandler(void)
{
    UART_DRV_IRQHandler(2);
}
#endif

#if (UART_INSTANCE_COUNT > 3U)
/*! @brief UART3 ISR - dispatches to UART_DRV_IRQHandler(3). */
void UART3_IrqHandler(void)
{
    UART_DRV_IRQHandler(3);
}
#endif

#if (UART_INSTANCE_COUNT > 4U)
/*! @brief UART4 ISR - dispatches to UART_DRV_IRQHandler(4). */
void UART4_IrqHandler(void)
{
    UART_DRV_IRQHandler(4);
}
#endif

#if (UART_INSTANCE_COUNT > 5U)
/*! @brief UART5 ISR - dispatches to UART_DRV_IRQHandler(5). */
void UART5_IrqHandler(void)
{
    UART_DRV_IRQHandler(5);
}
#endif

/*******************************************************************************
 * ISR Lookup Table
 ******************************************************************************/

/*!
 * @brief ISR function-pointer table, indexed by UART instance number.
 *
 * Referenced by UART_DRV_Init() to install the correct handler and by
 * UART_DRV_Deinit() to restore the default handler.
 */
isr_t g_uartIsr[UART_INSTANCE_COUNT] = /*PRQA S 1533*/
{
#if (UART_INSTANCE_COUNT > 0U)
    UART0_IrqHandler,
#endif
#if (UART_INSTANCE_COUNT > 1U)
    UART1_IrqHandler,
#endif
#if (UART_INSTANCE_COUNT > 2U)
    UART2_IrqHandler,
#endif
#if (UART_INSTANCE_COUNT > 3U)
    UART3_IrqHandler,
#endif
#if (UART_INSTANCE_COUNT > 4U)
    UART4_IrqHandler,
#endif
#if (UART_INSTANCE_COUNT > 5U)
    UART5_IrqHandler,
#endif
};

/*******************************************************************************
 * EOF
 ******************************************************************************/

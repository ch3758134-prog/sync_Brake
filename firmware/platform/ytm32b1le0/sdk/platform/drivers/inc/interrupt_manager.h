/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file interrupt_manager.h
 * @version 1.5.0
 *
 * @brief Interrupt Manager - Public API for NVIC and vector-table control.
 *
 * This header defines the application-facing services used to manage interrupt
 * routing through the core interrupt controller. It provides helpers for
 * installing RAM-based handlers, enabling or disabling IRQ lines, protecting
 * critical sections through nested global masking, programming priorities, and
 * accessing optional pending, active, software-trigger, or multi-core
 * interrupt services when supported by the target device.
 *
 * The APIs are organized into the following categories:
 *   - Handler Installation
 *   - IRQ Line Control
 *   - Global IRQ Control
 *   - Priority Control
 *   - Pending State Control
 *   - Active State Query
 *   - Software IRQ Control
 *   - Multi-Core IRQ Routing
 *
 * @note Runtime handler installation requires the active vector table to be
 *       copied to writable RAM.
 */

#ifndef INTERRUPT_MANAGER_H
#define INTERRUPT_MANAGER_H

#include "device_registers.h"

/*!
 * @addtogroup interrupt_manager
 * @brief Public API for vector-table updates and IRQ control.
 * @details Provides core-level interrupt services built on top of the CMSIS
 *          NVIC API and the active vector table selected by the startup code.
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief Enter a nested critical section by disabling global interrupts.
 *
 * This macro calls INT_SYS_DisableIRQGlobal() and increments the internal
 * nesting counter maintained by the Interrupt Manager.
 *
 * @note Pair every call with SDK_EXIT_CRITICAL().
 */
#define SDK_ENTER_CRITICAL() INT_SYS_DisableIRQGlobal()

/*!
 * @brief Exit a nested critical section and restore global interrupts when allowed.
 *
 * This macro calls INT_SYS_EnableIRQGlobal() and releases one nesting level.
 * Global interrupts are re-enabled only after the disable counter reaches zero.
 */
#define SDK_EXIT_CRITICAL() INT_SYS_EnableIRQGlobal()

/*!
 * @brief Interrupt service routine function pointer type.
 *
 * Use this type when installing a handler into the active vector table through
 * INT_SYS_InstallHandler().
 */
typedef void (*isr_t)(void);

/*******************************************************************************
 * Default Interrupt Handler
 ******************************************************************************/

/*! @brief Default fallback ISR supplied by the device startup code. */
void DefaultISR(void);

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * Handler Installation
 ******************************************************************************/
/*!
 * @name Handler Installation
 * @brief Functions for updating entries in the active vector table.
 * @{
 */

/*!
 * @brief Install a handler for the selected IRQ number.
 *
 * Replaces the current vector-table entry associated with @a irqNumber. The
 * function can optionally return the previous ISR pointer through
 * @a oldHandler so the caller can restore it later.
 *
 * @param[in] irqNumber   IRQ or exception number to update.
 * @param[in] newHandler  New ISR entry to place in the vector table.
 * @param[out] oldHandler Optional pointer used to store the previous ISR.
 *                        Pass `NULL` when the old entry is not needed.
 *
 * @note This service is valid only when the active vector table resides in
 *       writable RAM. If `__NO_VECTOR_TABLE_COPY` is defined, the startup file
 *       must provide the final handler bindings instead.
 */
void INT_SYS_InstallHandler(IRQn_Type irqNumber,
                            isr_t newHandler,
                            isr_t *oldHandler);

/*! @} */ /* End of Handler Installation */

/*******************************************************************************
 * IRQ Line Control
 ******************************************************************************/
/*!
 * @name IRQ Line Control
 * @brief Functions for enabling or disabling individual IRQ lines.
 * @{
 */

/*!
 * @brief Enable one IRQ line in the NVIC.
 *
 * @param[in] irqNumber IRQ number to enable.
 */
void INT_SYS_EnableIRQ(IRQn_Type irqNumber);

/*!
 * @brief Disable one IRQ line in the NVIC.
 *
 * @param[in] irqNumber IRQ number to disable.
 */
void INT_SYS_DisableIRQ(IRQn_Type irqNumber);

/*! @} */ /* End of IRQ Line Control */

/*******************************************************************************
 * Global IRQ Control
 ******************************************************************************/
/*!
 * @name Global IRQ Control
 * @brief Functions for entering and leaving nested global interrupt masking.
 * @{
 */

/*!
 * @brief Release one level of nested global interrupt masking.
 *
 * Decrements the internal disable counter. Global interrupts are re-enabled
 * only after the counter reaches zero.
 */
void INT_SYS_EnableIRQGlobal(void);

/*!
 * @brief Disable global interrupts and enter a nested critical section.
 *
 * Executes the core global-disable instruction and increments the internal
 * disable counter used by the Interrupt Manager.
 */
void INT_SYS_DisableIRQGlobal(void);

/*! @} */ /* End of Global IRQ Control */

/*******************************************************************************
 * Priority Control
 ******************************************************************************/
/*!
 * @name Priority Control
 * @brief Functions for programming or reading IRQ priorities.
 * @{
 */

/*!
 * @brief Set the priority of one IRQ or exception.
 *
 * @param[in] irqNumber IRQ or exception number to update.
 * @param[in] priority  Priority value to program.
 *
 * @note Some core exceptions expose fixed or restricted priority behavior.
 */
void INT_SYS_SetPriority(IRQn_Type irqNumber, uint8_t priority);

/*!
 * @brief Read the programmed priority of one IRQ or exception.
 *
 * @param[in] irqNumber IRQ or exception number to query.
 * @return Priority value reported by the core interrupt controller.
 *
 * @note Some core exceptions expose fixed or restricted priority behavior.
 */
uint8_t INT_SYS_GetPriority(IRQn_Type irqNumber);

/*! @} */ /* End of Priority Control */

#if FEATURE_INTERRUPT_HAS_PENDING_STATE

/*******************************************************************************
 * Pending State Control
 ******************************************************************************/
/*!
 * @name Pending State Control
 * @brief Functions for managing the pending state of one IRQ line.
 * @{
 */

/*!
 * @brief Clear the pending state of one IRQ line.
 *
 * @param[in] irqNumber IRQ number to acknowledge.
 */
void INT_SYS_ClearPending(IRQn_Type irqNumber);

/*!
 * @brief Force one IRQ line into the pending state.
 *
 * @param[in] irqNumber IRQ number to mark as pending.
 */
void INT_SYS_SetPending(IRQn_Type irqNumber);

/*!
 * @brief Read the pending state of one IRQ line.
 *
 * @param[in] irqNumber IRQ number to query.
 * @return Non-zero when the IRQ is pending, otherwise zero.
 */
uint32_t INT_SYS_GetPending(IRQn_Type irqNumber);

/*! @} */ /* End of Pending State Control */

#endif /* FEATURE_INTERRUPT_HAS_PENDING_STATE */

#if FEATURE_INTERRUPT_HAS_ACTIVE_STATE

/*******************************************************************************
 * Active State Query
 ******************************************************************************/
/*!
 * @name Active State Query
 * @brief Functions for querying whether an IRQ is currently active.
 * @{
 */

/*!
 * @brief Read the active state of one IRQ line.
 *
 * @param[in] irqNumber IRQ number to query.
 * @return Non-zero when the IRQ is active, otherwise zero.
 */
uint32_t INT_SYS_GetActive(IRQn_Type irqNumber);

/*! @} */ /* End of Active State Query */

#endif /* FEATURE_INTERRUPT_HAS_ACTIVE_STATE */

#if FEATURE_INTERRUPT_HAS_SOFTWARE_IRQ

/*******************************************************************************
 * Software IRQ Control
 ******************************************************************************/
/*!
 * @name Software IRQ Control
 * @brief Functions for triggering or clearing software-generated IRQ requests.
 * @{
 */

/*!
 * @brief Trigger a software-generated IRQ request.
 *
 * @param[in] irqNumber IRQ number to trigger through software.
 */
void INT_SYS_SetSoftwareIRQRequest(IRQn_Type irqNumber);

/*!
 * @brief Clear a software-generated IRQ request.
 *
 * @param[in] irqNumber IRQ number whose software request should be cleared.
 */
void INT_SYS_ClearSoftwareIRQRequest(IRQn_Type irqNumber);

/*! @} */ /* End of Software IRQ Control */

#endif /* FEATURE_INTERRUPT_HAS_SOFTWARE_IRQ */


#if FEATURE_INTERRUPT_MULTICORE_SUPPORT

/*******************************************************************************
 * Multi-Core IRQ Routing
 ******************************************************************************/
/*!
 * @name Multi-Core IRQ Routing
 * @brief Functions for enabling or disabling one IRQ line across cores.
 * @{
 */

/*!
 * @brief Enable one IRQ line on the selected cores.
 *
 * @param[in] irqNumber IRQ number to enable.
 * @param[in] coresIds  Array of core identifiers to update.
 * @param[in] coresCnt  Number of entries in @a coresIds.
 */
void INT_SYS_EnableIRQ_MC(IRQn_Type irqNumber,
                          const uint8_t *coresIds,
                          uint8_t coresCnt);

/*!
 * @brief Query which cores currently enable one IRQ line.
 *
 * @param[in] irqNumber IRQ number to inspect.
 * @param[out] cores    Caller-allocated array indexed by core number. Each
 *                      element receives `INTERRUPT_CORE_ENABLED` or
 *                      `INTERRUPT_CORE_DISABLED`.
 */
void INT_SYS_GetCoresForIRQ(IRQn_Type irqNumber, interrupt_core_enable_t *cores);

/*!
 * @brief Disable one IRQ line on the selected cores.
 *
 * @param[in] irqNumber IRQ number to disable.
 * @param[in] coresIds  Array of core identifiers to update.
 * @param[in] coresCnt  Number of entries in @a coresIds.
 */
void INT_SYS_DisableIRQ_MC(IRQn_Type irqNumber,
                           const uint8_t *coresIds,
                           uint8_t coresCnt);

/*!
 * @brief Disable one IRQ line on every supported core.
 *
 * @param[in] irqNumber IRQ number to disable globally across cores.
 */
void INT_SYS_DisableIRQ_MC_All(IRQn_Type irqNumber);

/*! @} */ /* End of Multi-Core IRQ Routing */

#endif /* FEATURE_INTERRUPT_MULTICORE_SUPPORT */


#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* INTERRUPT_MANAGER_H */

/*! @} */
/*******************************************************************************
 * EOF
 ******************************************************************************/

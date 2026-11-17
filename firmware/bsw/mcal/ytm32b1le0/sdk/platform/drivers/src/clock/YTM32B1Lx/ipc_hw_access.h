/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file ipc_hw_access.h
 * @version 1.5.0
 *
 * @brief IPC Hardware Access - Inter-Peripheral Crossbar clock control register interface.
 *
 * This header provides low-level inline functions for controlling peripheral
 * clock gates, clock sources, and dividers through the IPC (Inter-Peripheral
 * Crossbar) peripheral clock control registers.
 *
 * Functions are organized into:
 *   - **Clock Configuration** - Set peripheral clock source, divider, and gate.
 *   - **Module Reset** - Assert/de-assert software reset for a peripheral.
 *   - **Clock Gate Control** - Enable/disable individual peripheral clocks.
 *   - **Clock Status Query** - Read back clock gate, source, and divider settings.
 */

#ifndef IPC_HW_ACCESS_H
#define IPC_HW_ACCESS_H

#include "device_registers.h"

/*! @brief Clock name to IPC control register index mappings.
 *
 * Maps each clock_names_t value to its peripheral clock control register
 * index. Entries with value IPC_INVALID_INDEX indicate no IPC mapping.
 */
extern const uint16_t clockNameMappings[CLOCK_NAME_COUNT];

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*******************************************************************************
 * Clock Configuration
 ******************************************************************************/
/*!
 * @name Clock Configuration
 * @brief Set peripheral clock source, divider, and gate in a single write.
 * @{
 */

/*!
 * @brief Configure a peripheral clock control register.
 *
 * Writes the clock source, divider, and gate enable to the IPC control
 * register in a single register write.
 *
 * @param[in] ipcCtrlReg   Pointer to the IPC control register.
 * @param[in] clockGate    true to enable the peripheral clock, false to gate it.
 * @param[in] clockSource  Clock source selection value.
 * @param[in] divider      Clock divider value.
 */
static inline void IPC_SetPeripheralClockControl(volatile uint32_t *ipcCtrlReg, bool clockGate, uint32_t clockSource, uint32_t divider)
{
   /* Configure the peripheral clock source, the fractional clock divider and the clock gate */
   uint32_t value =  IPC_CTRL_SRCSEL(clockSource)              |
                     IPC_CTRL_DIV(divider)                  |
                     IPC_CTRL_CLKEN(clockGate ? 1U : 0U);

   *ipcCtrlReg = value;
}

/*! @} */ /* End of Clock Configuration */

/*******************************************************************************
 * Module Reset
 ******************************************************************************/
/*!
 * @name Module Reset
 * @brief Assert and de-assert software reset for a peripheral.
 * @{
 */

/*!
 * @brief Assert and de-assert software reset for a peripheral.
 *
 * Pulses the SWREN bit in the IPC control register to perform a
 * software reset of the peripheral.
 *
 * @param[in] ctrlReg  Pointer to the IPC control register.
 */
static inline void IPC_ResetModule(volatile uint32_t* ctrlReg)
{
    /* Assert reset */
    *ctrlReg |= IPC_CTRL_SWREN(1UL);
    /* De-assert reset */
    *ctrlReg &= (uint32_t)(~(IPC_CTRL_SWREN_MASK));
}

/*! @} */ /* End of Module Reset */

/*******************************************************************************
 * Clock Gate Control
 ******************************************************************************/
/*!
 * @name Clock Gate Control
 * @brief Enable or disable individual peripheral clocks.
 * @{
 */

/*!
 * @brief Enable or disable a peripheral clock gate.
 *
 * @param[in] ctrlReg        Pointer to the IPC control register.
 * @param[in] isClockEnabled true to enable clock, false to disable.
 */
static inline void IPC_SetClockMode(volatile uint32_t* ctrlReg,
                                       const bool isClockEnabled)
{
   if (isClockEnabled)
   {
       *ctrlReg |= IPC_CTRL_CLKEN(1UL);
   }
   else
   {
       *ctrlReg &= (uint32_t)(~(IPC_CTRL_CLKEN_MASK));
   }
}

/*! @} */ /* End of Clock Gate Control */

/*******************************************************************************
 * Clock Status Query
 ******************************************************************************/
/*!
 * @name Clock Status Query
 * @brief Read back clock gate, source, and divider settings.
 * @{
 */

/*!
 * @brief Get the clock gate status for a peripheral.
 *
 * @param[in] base       Pointer to the IPC peripheral base.
 * @param[in] clockName  Peripheral clock identifier.
 * @return true if clock is enabled, false if gated.
 */
static inline bool IPC_GetClockMode(const IPC_Type* const base,
                                       const clock_names_t clockName)
{
   uint32_t regValue = (uint32_t)base->CTRL[clockNameMappings[clockName]];
   regValue = (regValue & IPC_CTRL_CLKEN_MASK) >> IPC_CTRL_CLKEN_SHIFT;
   return (regValue == 0U) ? false : true;
}

/*!
 * @brief Get the clock source selection for a peripheral.
 *
 * @param[in] base       Pointer to the IPC peripheral base.
 * @param[in] clockName  Peripheral clock identifier.
 * @return Clock source selection value.
 */
static inline uint32_t IPC_GetClockSourceSel(const IPC_Type* const base,
                                                                 const clock_names_t clockName)
{
   return ((base->CTRL[clockNameMappings[clockName]] & IPC_CTRL_SRCSEL_MASK) >> IPC_CTRL_SRCSEL_SHIFT);
}

/*!
 * @brief Get the clock divider value for a peripheral.
 *
 * @param[in] base       Pointer to the IPC peripheral base.
 * @param[in] clockName  Peripheral clock identifier.
 * @return Clock divider value.
 */
static inline uint32_t IPC_GetDividerSel(const IPC_Type* const base,
                                             const clock_names_t clockName)
{
   return ((base->CTRL[clockNameMappings[clockName]] & IPC_CTRL_DIV_MASK) >> IPC_CTRL_DIV_SHIFT);
}

/*! @} */ /* End of Clock Status Query */

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* IPC_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

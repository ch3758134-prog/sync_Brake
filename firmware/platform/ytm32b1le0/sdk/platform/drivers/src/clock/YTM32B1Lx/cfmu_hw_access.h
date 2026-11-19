/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file cfmu_hw_access.h
 * @version 1.5.0
 *
 * @brief CFMU Hardware Access - Clock Frequency Measurement Unit register interface.
 *
 * This header provides low-level inline functions for controlling the CFMU
 * peripheral, which is used for clock output routing and clock frequency
 * measurement on devices with separate CMU and CLKOUT hardware.
 *
 * Functions are organized into:
 *   - **Clock Output Enable** - Enable/disable the clock measurement output.
 *   - **Clock Output Source** - Select the clock source to be measured.
 *   - **Clock Output Divider** - Set the output frequency divider.
 */

#ifndef CFMU_HW_ACCESS_H
#define CFMU_HW_ACCESS_H

#include "device_registers.h"
#include <stdbool.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*******************************************************************************
 * Clock Output Control
 ******************************************************************************/
/*!
 * @name Clock Output Control
 * @brief Functions for enabling, selecting, and dividing the CFMU clock output.
 * @{
 */

/*!
 * @brief Enable or disable the CFMU clock output.
 *
 * @param[in] base    Base address for current CFMU instance.
 * @param[in] enable  true to enable clock output, false to disable.
 */
static inline void CFMU_SetClkOutEnable(CFMU_Type *base, const bool enable)
{
#if defined(CFMU_CTRL_CLK_OUTEN_MASK)
    base->CTRL &= (uint32_t)(~(0x01 << CFMU_CTRL_CLK_OUTEN_SHIFT));
    base->CTRL |= (enable << CFMU_CTRL_CLK_OUTEN_SHIFT);
#else
    (void)base;
    (void)enable;
#endif
}

/*!
 * @brief Select the clock output source.
 *
 * @param[in] base  Base address for current CFMU instance.
 * @param[in] src   Clock source: 0=Disable, 1=FIRC, 2=SIRC, 3=FXOSC, 4=SXOSC, 5=LPO.
 */
static inline void CFMU_SetClkOutSrc(CFMU_Type* const base, const uint8_t src)
{
    uint32_t regValue = base->CTRL;
    regValue &= ~(CFMU_CTRL_CLK_SEL_MASK);
    regValue |= CFMU_CTRL_CLK_SEL(src);
    base->CTRL = regValue;
}

/*!
 * @brief Set the clock output divider.
 *
 * @param[in] base    Base address for current CFMU instance.
 * @param[in] clkDiv  Clock output divider value.
 */
static inline void CFMU_SetClkOutDiv(CFMU_Type *base, const uint8_t clkDiv)
{
    uint32_t regValue = base->CTRL;
    regValue &= (uint32_t)(~CFMU_CTRL_CLK_DIV_MASK);
    regValue |= (uint32_t)CFMU_CTRL_CLK_DIV(clkDiv);
    base->CTRL = regValue;
}

/*! @} */ /* End of Clock Output Control */

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* CFMU_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

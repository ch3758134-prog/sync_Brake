/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file cmu_hw_access.h
 * @version 1.5.0
 *
 * @brief CMU Hardware Access - Clock Monitor Unit register interface.
 *
 * This header provides low-level inline functions for controlling the
 * standalone CMU (Clock Monitor Unit) peripheral, used on devices where
 * CMU is a separate module from the SCU.
 *
 * Functions are organized into:
 *   - **Channel Enable & Reset** - Enable/disable CMU channels and reset-on-error.
 *   - **Reference Clock** - Select CMU channel reference clock source.
 *   - **Threshold Config** - Set high/low compare thresholds per channel.
 */

#ifndef CMU_HW_ACCESS_H
#define CMU_HW_ACCESS_H

#include "device_registers.h"
#include <stdbool.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*******************************************************************************
 * Channel Enable & Reset
 ******************************************************************************/
/*!
 * @name Channel Enable & Reset
 * @brief Enable/disable CMU monitoring channels and reset-on-error.
 * @{
 */

/*!
 * @brief Enable or disable a CMU monitoring channel.
 *
 * @param[in] base    Base address for current CMU instance.
 * @param[in] ch      CMU channel index.
 * @param[in] enable  true to enable, false to disable.
 */
static inline void CMU_SetChEnable(CMU_Type *base, const uint8_t ch, const bool enable)
{
    uint32_t regValue = base->CTRL;
    regValue &= (~((uint32_t)0x1U << (CMU_CTRL_EN_SHIFT + ch)));
    base->CTRL = regValue | ((uint32_t)(enable ? 1U : 0U) << (CMU_CTRL_EN_SHIFT + ch));
}

/*!
 * @brief Enable or disable system reset on CMU channel clock error.
 *
 * @param[in] base    Base address for current CMU instance.
 * @param[in] ch      CMU channel index.
 * @param[in] enable  true to enable reset on error, false to disable.
 */
static inline void CMU_SetChResetEnable(CMU_Type *base, const uint8_t ch, const bool enable)
{
    uint32_t regValue = base->CTRL;
    regValue &= (~((uint32_t)0x1U << (CMU_CTRL_RE_SHIFT + ch)));
    base->CTRL = regValue | ((uint32_t)(enable ? 1U : 0U) << (CMU_CTRL_RE_SHIFT + ch));
}

/*! @} */ /* End of Channel Enable & Reset */

/*******************************************************************************
 * Reference Clock
 ******************************************************************************/
/*!
 * @name Reference Clock
 * @brief Select the reference clock source for a CMU channel.
 * @{
 */

/*!
 * @brief Select the reference clock for a CMU channel.
 *
 * @param[in] base      Base address for current CMU instance.
 * @param[in] ch        CMU channel index.
 * @param[in] refClock  Reference clock: 0=SIRC, 1=FXOSC.
 */
static inline void CMU_SetChRefClock(CMU_Type *base, const uint8_t ch, const uint8_t refClock)
{
    uint32_t regValue = base->CTRL;
    regValue &= (~((uint32_t)0x1U << (CMU_CTRL_REFS_SHIFT + ch)));
    base->CTRL = regValue | ((uint32_t)refClock << (CMU_CTRL_REFS_SHIFT + ch));
}

/*! @} */ /* End of Reference Clock */

/*******************************************************************************
 * Threshold Config
 ******************************************************************************/
/*!
 * @name Threshold Config
 * @brief Set high/low compare thresholds for CMU monitoring channels.
 * @{
 */

/*!
 * @brief Set the high compare threshold for a CMU channel.
 *
 * @param[in] base   Base address for current CMU instance.
 * @param[in] ch     CMU channel index.
 * @param[in] value  High threshold compare value.
 */
static inline void CMU_SetChCmpHigh(CMU_Type *base, const uint8_t ch, const uint16_t value)
{
    base->CMP[ch].HIGH = value;
}

/*!
 * @brief Set the low compare threshold for a CMU channel.
 *
 * @param[in] base   Base address for current CMU instance.
 * @param[in] ch     CMU channel index.
 * @param[in] value  Low threshold compare value.
 */
static inline void CMU_SetChCmpLow(CMU_Type *base, const uint8_t ch, const uint16_t value)
{
    base->CMP[ch].LOW = value;
}

/*! @} */ /* End of Threshold Config */

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* CMU_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

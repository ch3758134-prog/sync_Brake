/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file scu_hw_access.h
 * @version 1.5.0
 *
 * @brief SCU Hardware Access - System Clock Unit register interface (YTM32B1Lx).
 *
 * This header provides low-level inline functions for programming the SCU
 * peripheral on the YTM32B1Lx device family. Covers system clock dividers,
 * clock source selection, oscillator control (FXOSC, SXOSC), PLL management,
 * IRC low-power retention, clock output routing, CMU monitoring (when
 * integrated into SCU), and divider-stable status query.
 *
 * Functions are organized into:
 *   - **System Clock Dividers** - Core/bus divider set/get.
 *   - **Clock Source Selection** - System clock mux control.
 *   - **Clock Output** - Clock-out source and divider (when CMU is in SCU).
 *   - **FXOSC Control** - Enable, bypass, gain, ALC, output delay.
 *   - **SXOSC Control** - Enable, bypass, gain, low-power retention.
 *   - **PLL Control** - Reference clock, dividers, enable, lock status.
 *   - **SIRC / FIRC Control** - IRC enable, low-power retention, valid status.
 *   - **Clock Valid Status** - FXOSC / SXOSC valid flags, divider stability.
 *   - **CMU Control** - Channel enable, reset, reference, thresholds (in-SCU variant).
 */

#ifndef SCU_HW_ACCESS_H
#define SCU_HW_ACCESS_H

#include "device_registers.h"
#include <stdbool.h>
#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*******************************************************************************
 * System Clock Dividers
 ******************************************************************************/
/*!
 * @name System Clock Dividers
 * @brief Set and get core / fast-bus / slow-bus clock dividers.
 * @{
 */

/*!
 * @brief Set the system (core/platform) clock divider.
 *
 * @param[in] base       Pointer to the SCU peripheral base.
 * @param[in] sysClkDiv  Divider value.
 */
static inline void SCU_SetSysClkDiv(SCU_Type* const base, uint8_t sysClkDiv)
{
    uint32_t regValue = base->DIV;
    regValue &= ~(SCU_DIV_CPDIVS_MASK);
    regValue |= SCU_DIV_CPDIVS(sysClkDiv);
    base->DIV = regValue;
}

/*!
 * @brief Get the current system (core/platform) clock divider.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return Current system clock divider value.
 */
static inline uint8_t SCU_GetSysClkDiv(const SCU_Type* base)
{
    return (uint8_t)((base->DIVSTS & SCU_DIVSTS_CPDIVST_MASK) >> SCU_DIVSTS_CPDIVST_SHIFT);
}

/*!
 * @brief Set the fast-bus clock divider.
 *
 * @param[in] base          Pointer to the SCU peripheral base.
 * @param[in] fastBusClkDiv Fast bus clock divider value.
 */
static inline void SCU_SetFastBusClkDiv(SCU_Type* const base, uint16_t fastBusClkDiv)
{
    uint32_t regValue = base->DIV;
    regValue &= ~(SCU_DIV_FBDIVS_MASK);
    regValue |= SCU_DIV_FBDIVS(fastBusClkDiv);
    base->DIV  = regValue;
}

/*!
 * @brief Get the current fast-bus clock divider.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return Current fast bus clock divider value.
 */
static inline uint8_t SCU_GetFastBusClkDiv(const SCU_Type* base)
{
    return (uint8_t)((base->DIVSTS & SCU_DIVSTS_FBDIVST_MASK) >> SCU_DIVSTS_FBDIVST_SHIFT);
}

/*!
 * @brief Set the slow-bus clock divider.
 *
 * @param[in] base          Pointer to the SCU peripheral base.
 * @param[in] slowBusClkDiv Slow bus clock divider value.
 */
static inline void SCU_SetSlowBusClkDiv(SCU_Type* const base, uint16_t slowBusClkDiv)
{
    uint32_t regValue = base->DIV;
    regValue &= ~(SCU_DIV_SBDIVS_MASK);
    regValue |= SCU_DIV_SBDIVS(slowBusClkDiv);
    base->DIV  = regValue;
}

/*!
 * @brief Get the current slow-bus clock divider.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return Current slow bus clock divider value.
 */
static inline uint8_t SCU_GetSlowBusClkDiv(const SCU_Type* base)
{
    return (uint8_t)((base->DIVSTS & SCU_DIVSTS_SBDIVST_MASK) >> SCU_DIVSTS_SBDIVST_SHIFT);
}

/*! @} */ /* End of System Clock Dividers */

/*******************************************************************************
 * Clock Source Selection
 ******************************************************************************/
/*!
 * @name Clock Source Selection
 * @brief Select and query the system clock source.
 * @{
 */

/*!
 * @brief Set the system clock source.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] clkSrc  Clock source: 0=FIRC, 1=PLL, 2=FXOSC, 3=SIRC.
 */
static inline void SCU_SetSysClkSrc(SCU_Type* const base, uint8_t clkSrc)
{
    uint32_t regValue = base->CLKS;
    regValue &= ~(SCU_CLKS_CLKSRCSEL_MASK);
    regValue |= SCU_CLKS_CLKSRCSEL(clkSrc);
    base->CLKS = regValue;
}

/*!
 * @brief Get the current system clock source.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return Current clock source: 0=FIRC, 1=PLL, 2=FXOSC, 3=SIRC.
 */
static inline uint8_t SCU_GetSysClkSrc(const SCU_Type* base)
{
    return (uint8_t)((base->STS & SCU_STS_CLKST_MASK) >> SCU_STS_CLKST_SHIFT);
}

/*! @} */ /* End of Clock Source Selection */

/*******************************************************************************
 * Clock Output
 ******************************************************************************/
/*!
 * @name Clock Output
 * @brief Clock-out source and divider (in-SCU CMU variant).
 * @{
 */

#if defined(FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT) && (FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT == 0)
/*!
 * @brief Select the clock-out source.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @param[in] src   Clock-out source: 0=Disable, 1=FIRC, 2=SIRC, 3=FXOSC, 4=SXOSC, 5=LPO.
 */
static inline void SCU_SetClkOutSrc(SCU_Type* const base, const uint8_t src)
{
    uint32_t regValue = base->CLKO;
    regValue &= ~(SCU_CLKO_CLKOSEL_MASK);
    regValue |= SCU_CLKO_CLKOSEL(src);
    base->CLKO = regValue;
}

/*!
 * @brief Set the clock-out divider.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] clkDiv  Clock-out divider value.
 */
static inline void SCU_SetClkOutDiv(SCU_Type *base, const uint8_t clkDiv)
{
    uint32_t regValue = base->CLKO;
    regValue &= (uint32_t)(~SCU_CLKO_CLKODIV_MASK);
    regValue |= (uint32_t)SCU_CLKO_CLKODIV(clkDiv);
    base->CLKO = regValue;
}
#endif

/*! @} */ /* End of Clock Output */

/*******************************************************************************
 * FXOSC Control
 ******************************************************************************/
/*!
 * @name FXOSC Control
 * @brief Enable, bypass, gain, ALC, and output delay for the fast external oscillator.
 * @{
 */

/*!
 * @brief Enable or disable the FXOSC.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] enable  true to enable, false to disable.
 */
static inline void SCU_EnableFxosc(SCU_Type* const base, const bool enable)
{
    uint32_t regValue = base->FXOSC_CTRL;
    regValue &= (uint32_t)(~(SCU_FXOSC_CTRL_FXOSC_EN_MASK));
    regValue |= (uint32_t)SCU_FXOSC_CTRL_FXOSC_EN(enable ? 1U : 0U);
    base->FXOSC_CTRL = regValue;
}

/*!
 * @brief Get the FXOSC enable flag.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return 1 if enabled, 0 if disabled.
 */
static inline uint8_t SCU_GetFxoscEnableFlag(const SCU_Type* base)
{
    return (uint8_t)(((base->FXOSC_CTRL) & SCU_FXOSC_CTRL_FXOSC_EN_MASK) >> SCU_FXOSC_CTRL_FXOSC_EN_SHIFT);
}

/*!
 * @brief Set FXOSC bypass mode (external clock input vs. crystal oscillator).
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] enable  true for external clock input, false for crystal mode.
 */
static inline void SCU_SetFxoscBypassMode(SCU_Type* const base, const bool enable)
{
    uint32_t regValue = base->FXOSC_CTRL;
    regValue &= (uint32_t)(~(SCU_FXOSC_CTRL_FXOSC_MODE_MASK));
    regValue |= (uint32_t)SCU_FXOSC_CTRL_FXOSC_MODE(enable ? 0U : 1U);
    base->FXOSC_CTRL = regValue;
}

/*!
 * @brief Set FXOSC output delay selection.
 *
 * @param[in] base   Pointer to the SCU peripheral base.
 * @param[in] delay  Delay selection value.
 */
static inline void SCU_SetFxoscOutDelay(SCU_Type* const base, const uint8_t delay)
{
    uint32_t regValue = base->FXOSC_CTRL;
    regValue &= (uint32_t)(~(SCU_FXOSC_CTRL_DLY_SEL_MASK));
    regValue |= (uint32_t)SCU_FXOSC_CTRL_DLY_SEL(delay);
    base->FXOSC_CTRL = regValue;
}

/*!
 * @brief Set the FXOSC gain selection.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @param[in] gain  Gain selection value.
 */
static inline void SCU_SetFxoscGain(SCU_Type* const base, const uint8_t gain)
{
    uint32_t regValue = base->FXOSC_CTRL;
    regValue &= (uint32_t)(~(SCU_FXOSC_CTRL_GMSEL_MASK));
    regValue |= (uint32_t)SCU_FXOSC_CTRL_GMSEL(gain);
    base->FXOSC_CTRL = regValue;
}

/*!
 * @brief Enable or disable FXOSC automatic gain control (ALC).
 *
 * @param[in] base             Pointer to the SCU peripheral base.
 * @param[in] autoGainDisable  true to disable ALC, false to enable.
 */
#if defined(SCU_FXOSC_CTRL_ALCDIS_MASK)
static inline void SCU_SetFxoscALC(SCU_Type* const base, const bool autoGainDisable)
{
    uint32_t regValue = base->FXOSC_CTRL;
    regValue &= (uint32_t)(~(SCU_FXOSC_CTRL_ALCDIS_MASK));
    regValue |= (uint32_t)SCU_FXOSC_CTRL_ALCDIS(autoGainDisable ? 1U : 0U);
    base->FXOSC_CTRL = regValue;
}
#endif /* SCU_FXOSC_CTRL_ALCDIS_MASK */

/*! @} */ /* End of FXOSC Control */

/*******************************************************************************
 * SXOSC Control
 ******************************************************************************/
/*!
 * @name SXOSC Control
 * @brief Enable, bypass, gain, and low-power retention for the slow external oscillator.
 * @{
 */

#if defined(FEATURE_SCU_SUPPORT_SXOSC) && (FEATURE_SCU_SUPPORT_SXOSC == 1)
/*!
 * @brief Enable or disable the SXOSC.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] enable  true to enable, false to disable.
 */
static inline void SCU_EnableSxosc(SCU_Type* const base, const bool enable)
{
    uint32_t regValue = base->SXOSC_CTRL;
    regValue &= (uint32_t)(~(SCU_SXOSC_CTRL_SXOSC_EN_MASK));
    regValue |= (uint32_t)SCU_SXOSC_CTRL_SXOSC_EN(enable ? 1U : 0U);
    base->SXOSC_CTRL = regValue;
}

/*!
 * @brief Enable or disable SXOSC during deep-sleep mode.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] enable  true to keep SXOSC running in deep-sleep.
 */
static inline void SCU_SxoscDeepSleepEnable(SCU_Type* const base, const bool enable)
{
    uint32_t regValue = base->SXOSC_CTRL;
    regValue &= (uint32_t)(~(SCU_SXOSC_CTRL_SXOSC_DS_EN_MASK));
    regValue |= (uint32_t)SCU_SXOSC_CTRL_SXOSC_DS_EN(enable ? 1U : 0U);
    base->SXOSC_CTRL = regValue;
}

/*!
 * @brief Enable or disable SXOSC during standby mode.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] enable  true to keep SXOSC running in standby.
 */
static inline void SCU_SxoscStandbyEnable(SCU_Type* const base, const bool enable)
{
    uint32_t regValue = base->SXOSC_CTRL;
    regValue &= (uint32_t)(~(SCU_SXOSC_CTRL_SXOSC_STB_EN_MASK));
    regValue |= (uint32_t)SCU_SXOSC_CTRL_SXOSC_STB_EN(enable ? 1U : 0U);
    base->SXOSC_CTRL = regValue;
}

/*!
 * @brief Get the SXOSC enable flag.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return 1 if enabled, 0 if disabled.
 */
static inline uint8_t SCU_GetSxoscEnableFlag(const SCU_Type* base)
{
    return (uint8_t)(((base->SXOSC_CTRL) & SCU_SXOSC_CTRL_SXOSC_EN_MASK) >> SCU_SXOSC_CTRL_SXOSC_EN_SHIFT);
}

/*!
 * @brief Set SXOSC bypass mode (external clock input vs. crystal).
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] enable  true for external clock, false for crystal mode.
 */
static inline void SCU_SetSxoscBypassMode(SCU_Type* const base, const bool enable)
{
    uint32_t regValue = base->SXOSC_CTRL;
    regValue &= (uint32_t)(~(SCU_SXOSC_CTRL_SXOSC_MODE_MASK));
    regValue |= (uint32_t)SCU_SXOSC_CTRL_SXOSC_MODE(enable ? 0U : 1U);
    base->SXOSC_CTRL = regValue;
}

/*!
 * @brief Set the SXOSC gain selection.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @param[in] gain  Gain selection value.
 */
static inline void SCU_SetSxoscGain(SCU_Type* const base, const uint8_t gain)
{
    uint32_t regValue = base->SXOSC_CTRL;
    regValue &= (uint32_t)(~(SCU_SXOSC_CTRL_SXOSC_HIGAIN_EN_MASK));
    regValue |= (uint32_t)SCU_SXOSC_CTRL_SXOSC_HIGAIN_EN(gain);
    base->SXOSC_CTRL = regValue;
}
#endif

/*! @} */ /* End of SXOSC Control */

/*******************************************************************************
 * PLL Control
 ******************************************************************************/
/*!
 * @name PLL Control
 * @brief PLL reference clock, dividers, enable, and lock status.
 * @{
 */

#if FEATURE_SCU_SUPPORT_PLL
/*!
 * @brief Set the PLL reference clock source.
 *
 * @param[in] base       Pointer to the SCU peripheral base.
 * @param[in] pllRefClk  Reference clock: 0=FXOSC, 1=FIRC.
 */
static inline void SCU_SetPllRefClk(SCU_Type* const base, const uint8_t pllRefClk)
{
    uint32_t regValue = base->PLL_CTRL;
    regValue &= (uint32_t)(~(SCU_PLL_CTRL_REFCLKSRCSEL_MASK));
    regValue |= (uint32_t)SCU_PLL_CTRL_REFCLKSRCSEL(pllRefClk);
    base->PLL_CTRL = regValue;
}

/*!
 * @brief Get the current PLL reference clock source.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return Reference clock source value.
 */
static inline uint8_t SCU_GetPllRefClk(const SCU_Type* base)
{
    return (uint8_t)((base->PLL_CTRL & SCU_PLL_CTRL_REFCLKSRCSEL_MASK) >> SCU_PLL_CTRL_REFCLKSRCSEL_SHIFT);
}

/*!
 * @brief Set the PLL feedback clock divider.
 *
 * @param[in] base           Pointer to the SCU peripheral base.
 * @param[in] pllFeedBackDiv Feedback divider value (hardware stores value-1).
 */
static inline void SCU_SetPllFeedBackDiv(SCU_Type* const base, const uint8_t pllFeedBackDiv)
{
    uint32_t regValue = base->PLL_CTRL;
    regValue &= (uint32_t)(~(SCU_PLL_CTRL_FBDIV_MASK));
    regValue |= (uint32_t)SCU_PLL_CTRL_FBDIV(pllFeedBackDiv - 1);
    base->PLL_CTRL = regValue;
}

/*!
 * @brief Get the current PLL feedback divider value.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return Feedback divider value (hardware value + 1).
 */
static inline uint8_t SCU_GetPllFeedBackDiv(const SCU_Type* base)
{
    return (uint8_t)(((base->PLL_CTRL & SCU_PLL_CTRL_FBDIV_MASK) >> SCU_PLL_CTRL_FBDIV_SHIFT) + 1);
}

/*!
 * @brief Set the PLL reference clock divider.
 *
 * @param[in] base         Pointer to the SCU peripheral base.
 * @param[in] pllRefClkDiv Reference clock divider value (hardware stores value-1).
 */
static inline void SCU_SetPllRefClkDiv(SCU_Type* const base, const uint8_t pllRefClkDiv)
{
    uint32_t regValue = base->PLL_CTRL;
    regValue &= (uint32_t)(~(SCU_PLL_CTRL_REFDIV_MASK));
    regValue |= (uint32_t)SCU_PLL_CTRL_REFDIV(pllRefClkDiv - 1);
    base->PLL_CTRL = regValue;
}

/*!
 * @brief Get the current PLL reference clock divider value.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return Reference clock divider value (hardware value + 1).
 */
static inline uint8_t SCU_GetPllRefClkDiv(const SCU_Type* base)
{
    return (uint8_t)(((base->PLL_CTRL & SCU_PLL_CTRL_REFDIV_MASK) >> SCU_PLL_CTRL_REFDIV_SHIFT) + 1);
}

/*!
 * @brief Enable or disable the PLL.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] enable  true to enable PLL, false to disable.
 */
static inline void SCU_EnablePll(SCU_Type* const base, const bool enable)
{
    uint32_t regValue = base->PLL_CTRL;
    regValue &= (uint32_t)(~(SCU_PLL_CTRL_PLL_EN_MASK));
    regValue |= (uint32_t)SCU_PLL_CTRL_PLL_EN(enable ? 1U : 0U);
    base->PLL_CTRL = regValue;
}

/*!
 * @brief Check whether the PLL is locked.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return true if PLL is locked.
 */
static inline bool SCU_GetPllLock(const SCU_Type* base)
{
    return ((base->STS & SCU_STS_PLL_LOCK_MASK) >> SCU_STS_PLL_LOCK_SHIFT) != 0U;
}
#endif /* FEATURE_SCU_SUPPORT_PLL */

/*! @} */ /* End of PLL Control */

/*******************************************************************************
 * SIRC / FIRC Control
 ******************************************************************************/
/*!
 * @name SIRC / FIRC Control
 * @brief IRC enable, low-power retention, and valid status flags.
 * @{
 */

/*!
 * @brief Enable or disable SIRC during deep-sleep mode.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] enable  true to force SIRC on during deep-sleep.
 */
static inline void SCU_SircDeepSleepEnable(SCU_Type *base, const bool enable)
{
    uint32_t regValue = base->SIRC_CTRL;
    regValue &= (uint32_t)(~SCU_SIRC_CTRL_SIRC_DS_EN_MASK);
    regValue |= (uint32_t)SCU_SIRC_CTRL_SIRC_DS_EN(enable ? 1U : 0U);
    base->SIRC_CTRL = regValue;
}

/*!
 * @brief Enable or disable SIRC during standby mode.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] enable  true to force SIRC on during standby.
 */
static inline void SCU_SircStandbyEnable(SCU_Type *base, const bool enable)
{
    uint32_t regValue = base->SIRC_CTRL;
    regValue &= (uint32_t)(~SCU_SIRC_CTRL_SIRC_STB_EN_MASK);
    regValue |= (uint32_t)SCU_SIRC_CTRL_SIRC_STB_EN(enable ? 1U : 0U);
    base->SIRC_CTRL = regValue;
}

#if defined(FEATURE_SOC_SUPPORT_POWERDOWN_MODE) && (FEATURE_SOC_SUPPORT_POWERDOWN_MODE == 1)
/*!
 * @brief Enable or disable SIRC during power-down mode.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] enable  true to force SIRC on during power-down.
 */
static inline void SCU_SircPowerDownEnable(SCU_Type *base, const bool enable)
{
    uint32_t regValue = base->SIRC_CTRL;
    regValue &= (uint32_t)(~SCU_SIRC_CTRL_SIRC_PD_EN_MASK);
    regValue |= (uint32_t)SCU_SIRC_CTRL_SIRC_PD_EN(enable ? 1U : 0U);
    base->SIRC_CTRL = regValue;
}

#endif /* FEATURE_SOC_SUPPORT_POWERDOWN_MODE */

/*!
 * @brief Enable or disable the FIRC.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] enable  true to enable FIRC, false to disable.
 */
static inline void SCU_EnableFirc(SCU_Type *base, const bool enable)
{
    uint32_t regValue = base->FIRC_CTRL;
    regValue &= (uint32_t)(~SCU_FIRC_CTRL_FIRC_EN_MASK);
    regValue |= (uint32_t)SCU_FIRC_CTRL_FIRC_EN(enable ? 1U : 0U);
    base->FIRC_CTRL = regValue;
}

/*!
 * @brief Enable or disable FIRC during deep-sleep mode.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] enable  true to force FIRC on during deep-sleep.
 */
static inline void SCU_FircDeepSleepEnable(SCU_Type *base, const bool enable)
{
    uint32_t regValue = base->FIRC_CTRL;
    regValue &= (uint32_t)(~SCU_FIRC_CTRL_FIRC_DS_EN_MASK);
    regValue |= (uint32_t)SCU_FIRC_CTRL_FIRC_DS_EN(enable ? 1U : 0U);
    base->FIRC_CTRL = regValue;
}

/*!
 * @brief Check whether the FIRC clock is valid.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return true if FIRC clock is valid.
 */
static inline bool SCU_GetFIRCValid(const SCU_Type* base)
{
    return ((base->STS & SCU_STS_FIRC_VALID_MASK) >> SCU_STS_FIRC_VALID_SHIFT) != 0U;
}

/*!
 * @brief Get the FIRC enable flag.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return 1 if FIRC is enabled, 0 if disabled.
 */
static inline uint8_t SCU_GetFircEnableFlag(const SCU_Type* base)
{
    return (uint8_t)(((base->FIRC_CTRL) & SCU_FIRC_CTRL_FIRC_EN_MASK) >> SCU_FIRC_CTRL_FIRC_EN_SHIFT);
}

/*! @} */ /* End of SIRC / FIRC Control */

/*******************************************************************************
 * Clock Valid Status
 ******************************************************************************/
/*!
 * @name Clock Valid Status
 * @brief FXOSC / SXOSC valid flags and divider stability query.
 * @{
 */

/*!
 * @brief Check whether the FXOSC clock is valid.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return true if FXOSC is valid.
 */
static inline bool SCU_GetFXOSCValid(const SCU_Type* base)
{
    return ((base->STS & SCU_STS_FXOSC_VALID_MASK) >> SCU_STS_FXOSC_VALID_SHIFT) != 0U;
}

#if defined(FEATURE_SCU_SUPPORT_SXOSC) && (FEATURE_SCU_SUPPORT_SXOSC == 1)
/*!
 * @brief Check whether the SXOSC clock is valid.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return true if SXOSC is valid.
 */
static inline bool SCU_GetSXOSCValid(const SCU_Type* base)
{
    return ((base->STS & SCU_STS_SXOSC_VALID_MASK) >> SCU_STS_SXOSC_VALID_SHIFT) != 0U;
}
#endif

/*!
 * @brief Check whether all clock dividers are stable.
 *
 * @param[in] base  Pointer to the SCU peripheral base.
 * @return true if dividers are stable (DIVSTS == DIV).
 */
static inline bool SCU_GetDividerStable(const SCU_Type* base)
{
    uint32_t regValue = base->DIVSTS;
    return (regValue == base->DIV);
}

/*! @} */ /* End of Clock Valid Status */

/*******************************************************************************
 * CMU Control (in-SCU variant)
 ******************************************************************************/
/*!
 * @name CMU Control (in-SCU variant)
 * @brief CMU channel enable, reset, reference clock, and thresholds when
 *        CMU is integrated inside the SCU.
 * @{
 */

#if defined(FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT) && (FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT == 0)
/*!
 * @brief Enable or disable a CMU monitoring channel.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] ch      CMU channel index.
 * @param[in] enable  true to enable, false to disable.
 */
static inline void SCU_SetCmuChEnable(SCU_Type *base, const uint8_t ch, const bool enable)
{
    uint32_t regValue = base->CMU_CTRL;
    regValue &= (~((uint32_t)0x01U << (SCU_CMU_CTRL_CMU0_EN_SHIFT + ch)));
    base->CMU_CTRL = regValue | ((uint32_t)(enable ? 1U : 0U) << (SCU_CMU_CTRL_CMU0_EN_SHIFT + ch));
}

/*!
 * @brief Enable or disable system reset on CMU channel clock error.
 *
 * @param[in] base    Pointer to the SCU peripheral base.
 * @param[in] ch      CMU channel index.
 * @param[in] enable  true to enable reset on error, false to disable.
 */
static inline void SCU_SetCmuChResetEnable(SCU_Type *base, const uint8_t ch, const bool enable)
{
    uint32_t regValue = base->CMU_CTRL;
    regValue &= (~((uint32_t)0x1U << (SCU_CMU_CTRL_CMU0_RE_SHIFT + ch)));
    base->CMU_CTRL = regValue | ((uint32_t)(enable ? 1U : 0U) << (SCU_CMU_CTRL_CMU0_RE_SHIFT + ch));
}

#if defined(FEATURE_SCU_CMU_SUPPORT_REF_CLOCK_SEL) && (FEATURE_SCU_CMU_SUPPORT_REF_CLOCK_SEL == 1)
/*!
 * @brief Select the reference clock for a CMU channel.
 *
 * @param[in] base      Pointer to the SCU peripheral base.
 * @param[in] ch        CMU channel index.
 * @param[in] refClock  Reference clock: 0=SIRC, 1=FXOSC.
 */
static inline void SCU_SetCmuChRefClock(SCU_Type *base, const uint8_t ch, const uint8_t refClock)
{
    uint32_t regValue = base->CMU_CTRL;
    regValue &= (uint32_t)(~(0x01 << (SCU_CMU_CTRL_CMU0_REFS_SHIFT + ch)));
    base->CMU_CTRL = regValue | (refClock << (SCU_CMU_CTRL_CMU0_REFS_SHIFT + ch));
}
#endif

/*!
 * @brief Set the high compare threshold for a CMU channel.
 *
 * @param[in] base   Pointer to the SCU peripheral base.
 * @param[in] ch     CMU channel index.
 * @param[in] value  High threshold compare value.
 */
static inline void SCU_SetCmuChCompHigh(SCU_Type *base, const uint8_t ch, const uint16_t value)
{
    base->CMUCMP[ch].HIGH = value;
}

/*!
 * @brief Set the low compare threshold for a CMU channel.
 *
 * @param[in] base   Pointer to the SCU peripheral base.
 * @param[in] ch     CMU channel index.
 * @param[in] value  Low threshold compare value.
 */
static inline void SCU_SetCmuChCompLow(SCU_Type *base, const uint8_t ch, const uint16_t value)
{
    base->CMUCMP[ch].LOW = value;
}
#endif

/*! @} */ /* End of CMU Control (in-SCU variant) */

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

#endif /* SCU_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

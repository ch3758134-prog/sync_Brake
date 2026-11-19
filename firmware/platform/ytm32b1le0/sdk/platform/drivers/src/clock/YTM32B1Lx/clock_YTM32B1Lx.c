/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file clock_YTM32B1Lx.c
 * @version 1.5.0
 *
 * @brief Clock Manager - YTM32B1Lx device-specific driver implementation.
 *
 * This file implements the Clock Manager for the YTM32B1Lx device family.
 * It configures the SCU (system clock source, dividers, FXOSC, SXOSC, PLL),
 * CMU (Clock Monitor Unit), IPC (peripheral clock gates, sources, dividers),
 * and provides the Clock Manager notification framework (init, update,
 * get-frequency) for application-level clock management.
 *
 * Functions are organized into:
 *   - **Internal Types** - Module-private enumerations (CMU channels, system
 *     clock type).
 *   - **Internal Forward Declarations** - Prototypes for static helpers.
 *   - **Driver Initialization** - CLOCK_DRV_Init and SCU/IPC/CMU sub-configs.
 *   - **Oscillator Wait Helpers** - Polling loops for PLL lock, divider
 *     stability, FIRC/FXOSC valid.
 *   - **IPC Configuration** - Peripheral clock gate and source setup.
 *   - **Module Reset & Module Clock** - Direct register access wrappers.
 *   - **Frequency Query** - CLOCK_DRV_GetFreq, CLOCK_DRV_GetPllFreq.
 *   - **Clock Manager Framework** - CLOCK_SYS_Init, UpdateConfiguration,
 *     GetCurrentConfiguration, GetErrorCallback, GetFreq, SetConfiguration.
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 1504 Rule 8.7: The object '%1s' is only referenced in the translation unit where it is defined.
 * PRQA S 2996 Rule 2.2: The result of this logical operation is always 'false'.
 */

#include "device_registers.h"
#include "ipc_hw_access.h"
#include "scu_hw_access.h"
#include "clock.h"
#include "interrupt_manager.h"
#include <stddef.h>


#if defined(FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT) && (FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT == 1)
#include "cfmu_hw_access.h"
#include "cmu_hw_access.h"
#endif

/*******************************************************************************
 * Internal Types
 ******************************************************************************/

static clock_manager_state_t g_clockState;

/*! @brief Cached FXOSC clock frequency (Hz). */
static uint32_t g_fxoscClkFreq;                         /* FXOSC clock */
#if defined(FEATURE_SCU_SUPPORT_SXOSC) && (FEATURE_SCU_SUPPORT_SXOSC == 1)
/*! @brief Cached SXOSC clock frequency (Hz). */
static uint32_t g_sxoscClkFreq;                         /* FXOSC clock */
#endif

/*! @brief Clock name to IPC index mapping table.
 *
 * Entries with value IPC_INVALID_INDEX indicate no IPC control register
 * exists for that clock name.
 */
const uint16_t clockNameMappings[] = IPC_CLOCK_NAME_MAPPINGS; /*PRQA S 1504*/

/*!
 * @brief SCU CMU monitored channel identifiers.
 */
typedef enum
{
    SCU_CMU_CHANNEL_FIRC_CLK = 0x00U,        /*!< CMU checks FIRC clock.   */
    SCU_CMU_CHANNEL_FXOSC_CLK = 0x01U        /*!< CMU checks FOXOSC clock. */
} scu_cmu_channel_t;

/*!
 * @brief SCU system clock domain identifiers.
 */
typedef enum
{
    SCU_SYSTEM_CLOCK_SYSTEM,        /*!< System clock.      */
    SCU_SYSTEM_CLOCK_FAST_BUS,      /*!< Fast bus clock.    */
    SCU_SYSTEM_CLOCK_SLOW_BUS,      /*!< Slow bus clock.    */
    SCU_SYSTEM_CLOCK_MAX            /*!< Max value.         */
} scu_system_clock_type_t;


/*******************************************************************************
 * Internal Forward Declarations
 ******************************************************************************/
static status_t CLOCK_SYS_SetScuConfiguration(const scu_config_t * scuConfig,
                                              const cmu_config_t* cmuConfig);
static inline void CLOCK_SYS_SetCmuChannelConfiguration(const cmu_ch_config_t* cmuChConfig,
                                                        const scu_cmu_channel_t channel);
static status_t CLOCK_SYS_ConfigureSystemClock(scu_system_clock_src_t clkSrc);
#if FEATURE_SCU_SUPPORT_PLL
static status_t CLOCK_SCU_WaitPllLock(void);
status_t CLOCK_DRV_GetPllFreq(uint32_t * frequency);
#endif /* FEATURE_SCU_SUPPORT_PLL */
static status_t CLOCK_SCU_WaitDividerStable(void);
static status_t CLOCK_SCU_WaitFIRCValid(void);
static void CLOCK_SYS_SetIpcConfiguration(const ipc_config_t * peripheralClockConfig);
static inline volatile uint32_t *CLOCK_GetIpcCtrlReg(clock_names_t clockName);
static void CLOCK_SYS_DisableCmu(void);
#if defined(FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT) && (FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT == 0)
static void CLOCK_SYS_ConfigureClkOut(const scu_config_t * scuConfig);
static void CLOCK_SYS_DisableClkOut(void);
#endif /* FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT */
static inline void CLOCK_SYS_SetDefaultCmuChannelConfiguration(uint32_t channelFreq, scu_cmu_channel_t  channel);

/*******************************************************************************
 * Driver Initialization
 ******************************************************************************/
/*!
 * @name Driver Initialization
 * @{
 */

/*!
 * @brief Initialize the clock system from a user configuration.
 *
 * Applies IPC peripheral and SCU system clock settings.
 *
 * @param[in] config  Pointer to the clock user configuration.
 * @return STATUS_SUCCESS on success; error code otherwise.
 */
status_t CLOCK_DRV_Init(clock_user_config_t const * config)
{
    status_t result = STATUS_SUCCESS;

    DEV_ASSERT(config != NULL);

    /* Set IPC settings before clock update. */
    CLOCK_SYS_SetIpcConfiguration(&config->ipcConfig);

    if (config->scuConfigPtr != NULL)
    {
        /* Set SCU settings. */
        result = CLOCK_SYS_SetScuConfiguration(config->scuConfigPtr, config->cmuConfigPtr);
    }

    return result;
}

/*!
 * @brief Disable all CMU monitoring channels.
 */
static void CLOCK_SYS_DisableCmu(void)
{
#if defined(FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT) && (FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT == 0)
    /* Disable CMU. */
    SCU->CMU_CTRL = 0;
    SCU->CMUSTS = SCU->CMUSTS;
#else
    /* Disable CMU. */
    CMU0->CTRL = 0;
    CMU0->STS = CMU0->STS;
#endif
}

#if defined(FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT) && (FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT == 0)
/*!
 * @brief Configure clock output from SCU (in-SCU CLK_OUT variant).
 *
 * @param[in] scuConfig  Pointer to the SCU configuration.
 */
static void CLOCK_SYS_ConfigureClkOut(const scu_config_t * scuConfig)
{
    SCU_SetClkOutSrc(SCU, (uint8_t)scuConfig->clockOutConfig.source);
    SCU_SetClkOutDiv(SCU, scuConfig->clockOutConfig.divider);
}

/*!
 * @brief Disable clock output from SCU.
 */
static void CLOCK_SYS_DisableClkOut(void)
{
    SCU_SetClkOutSrc(SCU, (uint8_t)SCU_CLKOUT_SEL_DISABLE);
}
#endif /* FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT */

/*!
 * @brief Configure SCU and CMU subsystems.
 *
 * Sequences through SIRC fallback, oscillator bringup, PLL lock (if supported),
 * divider configuration, and system clock switch.
 *
 * @param[in] scuConfig  Pointer to the SCU configuration.
 * @param[in] cmuConfig  Pointer to the CMU configuration (may be NULL).
 * @return STATUS_SUCCESS on success; error code otherwise.
 */
static status_t CLOCK_SYS_SetScuConfiguration(const scu_config_t * scuConfig,
                                              const cmu_config_t* cmuConfig)
{
    status_t retVal = STATUS_SUCCESS;
    uint32_t tempReg;
    uint32_t tempFreq;
    uint8_t state = 0U;
    if (NULL != scuConfig)
    {
        /* Only update when the SCU configuration is valid. */
        /* Disable CMU before updating the SCU registers. */
        CLOCK_SYS_DisableCmu();
        /* First switch to SIRC clock. */
        retVal = CLOCK_SYS_ConfigureSystemClock(SCU_SYSTEM_CLOCK_SRC_SIRC);
        if (STATUS_SUCCESS == retVal)
        {
            /* SIRC low power mode enable configuration */
            SCU_SircDeepSleepEnable(SCU, scuConfig->sircDeepSleepEnable);
            SCU_SircStandbyEnable(SCU, scuConfig->sircStandbyEnable);

            /* FIRC enable configuration */
            SCU_EnableFirc(SCU, scuConfig->fircEnable);
            SCU_FircDeepSleepEnable(SCU, scuConfig->fircDeepSleepEnable);
            if (cmuConfig != NULL)
            {
                retVal = CLOCK_SCU_WaitFIRCValid();
                if (retVal == STATUS_SUCCESS)
                {
                    /* Set CMU settings for FIRC. */
                    CLOCK_SYS_SetCmuChannelConfiguration(&(cmuConfig->fircClockMonitor),SCU_CMU_CHANNEL_FIRC_CLK);
                }
            }
            else
            {
                CLOCK_SYS_SetDefaultCmuChannelConfiguration(FEATURE_SCU_FIRC_FREQ, SCU_CMU_CHANNEL_FIRC_CLK);
            }

#if defined(FEATURE_SCU_SUPPORT_SXOSC) && (FEATURE_SCU_SUPPORT_SXOSC == 1)
            if (scuConfig->sxoscConfig.enable)
            {
                SCU_SetSxoscGain(SCU, scuConfig->sxoscConfig.gainSelection);
                SCU_SetSxoscBypassMode(SCU, scuConfig->sxoscConfig.bypassMode);
                SCU_SxoscDeepSleepEnable(SCU, scuConfig->sxoscConfig.deepsleepEnable);
                SCU_SxoscStandbyEnable(SCU, scuConfig->sxoscConfig.standbyEnable);
                SCU_EnableSxosc(SCU, scuConfig->sxoscConfig.enable);
                g_sxoscClkFreq = scuConfig->sxoscConfig.frequency;
            }
            else
            {
                /* SXOSC configuration */
                SCU_EnableSxosc(SCU, false);
                g_sxoscClkFreq = 0;
            }
#endif

            if (scuConfig->fxoscConfig.enable)
            {
                SCU_SetFxoscGain(SCU, scuConfig->fxoscConfig.gainSelection);
#if defined(SCU_FXOSC_CTRL_ALCDIS_MASK)
                /* Configure FXOSC automatic gain control (ALC). Default is enabled when autoGainDisable is false. */
                SCU_SetFxoscALC(SCU, scuConfig->fxoscConfig.autoGainDisable);
#endif /* SCU_FXOSC_CTRL_ALCDIS_MASK */
                SCU_SetFxoscBypassMode(SCU, scuConfig->fxoscConfig.bypassMode);
                SCU_EnableFxosc(SCU, scuConfig->fxoscConfig.enable);
                retVal = CLOCK_SYS_WaitFXOSCValid();
                if (STATUS_SUCCESS == retVal)
                {
                    g_fxoscClkFreq = scuConfig->fxoscConfig.frequency;
                    state = 1U;
                }
                else
                {
                    SCU_EnableFxosc(SCU, false);
                    g_fxoscClkFreq = 0;
                }

                if (cmuConfig != NULL)
                {
                    /* Set CMU settings for FXOSC. */
                    CLOCK_SYS_SetCmuChannelConfiguration(&(cmuConfig->fxoscClockMonitor),SCU_CMU_CHANNEL_FXOSC_CLK);
                }
                else
                {
                    CLOCK_SYS_SetDefaultCmuChannelConfiguration(g_fxoscClkFreq, SCU_CMU_CHANNEL_FXOSC_CLK);
                }
            }
            else
            {
                /* FXOSC configuration */
                SCU_EnableFxosc(SCU, false);
                g_fxoscClkFreq = 0;
                state = 1U;
            }


            if (state == 1U)
            {
#if defined(FEATURE_SCU_SUPPORT_PLL) && (FEATURE_SCU_SUPPORT_PLL == 1)
                /* PLL configuration */
                SCU_EnablePll(SCU, false);
                if (scuConfig->pllConfig.enable)
                {
                    SCU_SetPllRefClk(SCU, scuConfig->pllConfig.pllRefClock);
                    SCU_SetPllFeedBackDiv(SCU, scuConfig->pllConfig.pllFeedBackDiv);
                    SCU_SetPllRefClkDiv(SCU, scuConfig->pllConfig.pllRefClkDiv);
                    SCU_EnablePll(SCU, scuConfig->pllConfig.enable);
                    /* Wait pll lock */
                    retVal = CLOCK_SCU_WaitPllLock();
                    if (STATUS_SUCCESS != retVal)
                    {
                        /* PLL lock timeout */
                        SCU->PLL_CTRL = SCU_PLL_CTRL_FBDIV(1);
                        state = 0U;
                    }
                    else
                    {
                        state = 2U;
                    }

                    if (cmuConfig != NULL)
                    {
                        /* Set CMU settings for PLL. */
                        CLOCK_SYS_SetCmuChannelConfiguration(&(cmuConfig->pllClockMonitor), SCU_CMU_CHANNEL_PLL_CLK);
                    }
                    else
                    {
                        CLOCK_DRV_GetPllFreq(&tempFreq);
                        CLOCK_SYS_SetDefaultCmuChannelConfiguration(tempFreq, SCU_CMU_CHANNEL_PLL_CLK);
                    }
                }

                if (state != 0U)
#endif /* FEATURE_SCU_SUPPORT_PLL */
                {
                    /* Update System clock divider */
                    SCU_SetSysClkDiv(SCU, scuConfig->sysDiv);
                    SCU_SetFastBusClkDiv(SCU, scuConfig->fastBusDiv);
                    SCU_SetSlowBusClkDiv(SCU, scuConfig->slowBusDiv);
                    /* Update EFM divider */
                    tempReg = EFM->CTRL;
                    tempReg &= ~(EFM_CTRL_RWS_MASK);
                    EFM->CTRL = tempReg | EFM_CTRL_RWS(scuConfig->flashDiv);
#if defined(EFM_CTRL_DPD_EN_MASK)
                    EFM->CTRL |= EFM_CTRL_DPD_EN_MASK;
#endif
#if defined(EFM_CTRL_LPEN_MASK) && defined(EFM_CTRL_FWUP_EN_MASK)
                    EFM->CTRL |= EFM_CTRL_LPEN_MASK | EFM_CTRL_FWUP_EN_MASK;
#endif
                    /* Wait divider stable */
                    /* Switch to target clock */
                    retVal = CLOCK_SCU_WaitDividerStable();
                    if (retVal == STATUS_SUCCESS)
                    {
                        /* Switch to target clock */
                        retVal = CLOCK_SYS_ConfigureSystemClock(scuConfig->sysClkSrc);
                        if (retVal == STATUS_SUCCESS)
                        {
#if defined(FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT) && (FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT == 0)
                            /* config clock output */
                            if (scuConfig->clockOutConfig.enable)
                            {
                                CLOCK_SYS_ConfigureClkOut(scuConfig);
                            }
                            else
                            {
                                /* clock out disabled */
                                CLOCK_SYS_DisableClkOut();
                            }
#endif
                        }
                    }
                }
            }
        }
    }
    /* Update system clock frequency */
    (void)CLOCK_SYS_GetFreq(CORE_CLK, &SystemCoreClock);

    /* Update EFM prescaler clock frequency */
    (void)CLOCK_SYS_GetFreq(FEATURE_EFM_OPERATION_CLOCK_SOURCE, &tempFreq);
    /* Configure flash prescaler */
    tempReg = EFM->CTRL;
    tempReg &= ~(EFM_CTRL_PRESCALER_MASK);
#if defined(CPU_YTM32B1LE0) || defined(CPU_YTM32Z1LS0)
    EFM->CTRL = tempReg | EFM_CTRL_PRESCALER(tempFreq/1000000U);
#else
    EFM->CTRL = tempReg | EFM_CTRL_PRESCALER(tempFreq/2000000U);
#endif

    return retVal;
}

/*! @} */ /* End of Driver Initialization */

/*******************************************************************************
 * CMU Configuration Helpers
 ******************************************************************************/
/*!
 * @name CMU Configuration Helpers
 * @{
 */

/*!
 * @brief Configure default CMU thresholds and enable a channel.
 *
 * Uses a +/-25% tolerance band based on the SIRC reference to set compare
 * thresholds when no explicit CMU configuration is provided.
 *
 * @param[in] channelFreq  Expected frequency of the monitored clock (Hz).
 * @param[in] channel      CMU channel identifier.
 */
static inline void CLOCK_SYS_SetDefaultCmuChannelConfiguration(uint32_t channelFreq, scu_cmu_channel_t channel)
{
#ifdef CMU_ALWAYS_ON
    /* 25% offset for cmu default configuration */
    uint16_t compareHigh = (uint16_t)(channelFreq/1000000U) * 128U * 5U / 4U / (FEATURE_SCU_SIRC_FREQ/1000000U);
    uint16_t compareLow = (uint16_t)(channelFreq/1000000U) * 128U * 3U / 4U / (FEATURE_SCU_SIRC_FREQ/1000000U);

#if defined(FEATURE_SCU_SUPPORT_CMU_REFCLK_SEL) && (FEATURE_SCU_SUPPORT_CMU_REFCLK_SEL)
    CMU_SetChRefClock(CMU0, channel, CMU_REF_SIRC_CLOCK);
#endif
#if defined(FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT) && (FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT == 0)
    /* set compare value */
    SCU_SetCmuChCompHigh(SCU, (uint8_t)channel, compareHigh);
    SCU_SetCmuChCompLow(SCU, (uint8_t)channel, compareLow);
    /* set error reset enable */
    SCU_SetCmuChResetEnable(SCU, (uint8_t)channel, true);
    /* set enable */
    SCU_SetCmuChEnable(SCU, (uint8_t)channel, true);
#else
    /* set compare value */
    CMU_SetChCmpHigh(CMU0, (uint8_t)channel, compareHigh);
    CMU_SetChCmpLow(CMU0, (uint8_t)channel, compareLow);
    /* set reset enable */
    CMU_SetChResetEnable(CMU0, (uint8_t)channel, true);
    /* set enable */
    CMU_SetChEnable(CMU0, (uint8_t)channel, true);
#endif /* FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT */
#else
    (void)channelFreq;
    (void)channel;
#endif /* CMU_CONFIG_ALWAYS_ON */
}

/*!
 * @brief Configure a CMU channel from an explicit channel configuration.
 *
 * @param[in] cmuChConfig  Pointer to the channel configuration.
 * @param[in] channel      CMU channel identifier.
 */
static inline void CLOCK_SYS_SetCmuChannelConfiguration(const cmu_ch_config_t * cmuChConfig,
                                                        const scu_cmu_channel_t channel)
{
    if (NULL != cmuChConfig)
    {
#if defined(FEATURE_SCU_SUPPORT_CMU_REFCLK_SEL) && (FEATURE_SCU_SUPPORT_CMU_REFCLK_SEL)
        CMU_SetChRefClock(CMU0, channel, cmuChConfig->refClock);
#endif
#if defined(FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT) && (FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT == 0)
        /* set compare value */
        SCU_SetCmuChCompHigh(SCU, (uint8_t)channel, cmuChConfig->compareHigh);
        SCU_SetCmuChCompLow (SCU, (uint8_t)channel, cmuChConfig->compareLow);
        /* set reset enable */
        SCU_SetCmuChResetEnable (SCU, (uint8_t)channel, cmuChConfig->resetEnable);
        /* set enable */
        SCU_SetCmuChEnable (SCU, (uint8_t)channel, cmuChConfig->enable);
#else
        /* set compare value */
        CMU_SetChCmpHigh(CMU0, (uint8_t)channel, cmuChConfig->compareHigh);
        CMU_SetChCmpLow(CMU0, (uint8_t)channel, cmuChConfig->compareLow);
        /* set reset enable */
        CMU_SetChResetEnable(CMU0, (uint8_t)channel, cmuChConfig->resetEnable);
        /* set enable */
        CMU_SetChEnable(CMU0, (uint8_t)channel, cmuChConfig->enable);
#endif
    }
}

/*! @} */ /* End of CMU Configuration Helpers */

/*******************************************************************************
 * Oscillator Wait Helpers
 ******************************************************************************/
/*!
 * @name Oscillator Wait Helpers
 * @{
 */

/*!
 * @brief Switch to a system clock source and wait for completion.
 *
 * @param[in] clkSrc  Target system clock source.
 * @return STATUS_SUCCESS if switch completed; STATUS_SCU_CLK_SWITCH_TIMEOUT on timeout.
 */
static status_t CLOCK_SYS_ConfigureSystemClock(scu_system_clock_src_t clkSrc)
{
    status_t status = STATUS_SCU_CLK_SWITCH_TIMEOUT;
    uint32_t timeoutLoopCnt;
    SCU_SetSysClkSrc(SCU, (uint8_t)clkSrc);
    for(timeoutLoopCnt = 0; timeoutLoopCnt < FEATURE_CLOCK_TIMEOUT_LOOP_CNT; timeoutLoopCnt++)
    {
        if ((uint8_t)clkSrc == SCU_GetSysClkSrc(SCU))
        {
            status = STATUS_SUCCESS;
            break;
        }
    }
    return status;
}

#if FEATURE_SCU_SUPPORT_PLL
/*!
 * @brief Wait for the PLL to achieve lock.
 *
 * @return STATUS_SUCCESS if locked; STATUS_SCU_PLL_LOCK_TIMEOUT on timeout.
 */
static status_t CLOCK_SCU_WaitPllLock(void)
{
    status_t status = STATUS_SCU_PLL_LOCK_TIMEOUT;
    uint32_t timeoutLoopCnt;
    for(timeoutLoopCnt = 0; timeoutLoopCnt < FEATURE_CLOCK_TIMEOUT_LOOP_CNT; timeoutLoopCnt++)
    {
        if (SCU_GetPllLock(SCU))
        {
            status = STATUS_SUCCESS;
            break;
        }
    }
    return status;
}
#endif /* FEATURE_SCU_SUPPORT_PLL */

/*!
 * @brief Wait for system clock dividers to become stable.
 *
 * @return STATUS_SUCCESS if stable; STATUS_SCU_DIVIDER_SWITCH_TIMEOUT on timeout.
 */
static status_t CLOCK_SCU_WaitDividerStable(void)
{
    status_t status = STATUS_SCU_DIVIDER_SWITCH_TIMEOUT;
    uint32_t timeoutLoopCnt;
    for(timeoutLoopCnt = 0; timeoutLoopCnt < FEATURE_CLOCK_TIMEOUT_LOOP_CNT; timeoutLoopCnt++)
    {
        if (SCU_GetDividerStable(SCU))
        {
            status = STATUS_SUCCESS;
            break;
        }
    }
    return status;
}

/*!
 * @brief Wait for the FIRC clock to become valid.
 *
 * @return STATUS_SUCCESS if valid; STATUS_SCU_FIRC_READY_TIMEOUT on timeout.
 */
static status_t CLOCK_SCU_WaitFIRCValid(void)
{
    status_t status = STATUS_SCU_FIRC_READY_TIMEOUT;
    uint32_t timeoutLoopCnt;
    for(timeoutLoopCnt = 0; timeoutLoopCnt < FEATURE_CLOCK_TIMEOUT_LOOP_CNT; timeoutLoopCnt++)
    {
        if (SCU_GetFIRCValid(SCU))
        {
            status = STATUS_SUCCESS;
            break;
        }
    }
    return status;
}

/*!
 * @brief Wait for FXOSC to become valid, sweeping gain if necessary.
 *
 * @return STATUS_SUCCESS if FXOSC is valid; STATUS_SCU_FXOSC_READY_TIMEOUT on timeout.
 */
status_t CLOCK_SYS_WaitFXOSCValid(void)
{
    uint32_t timeoutLoopCnt;
    uint32_t regValue;
    int8_t fxosc_gain = 1;
    int8_t fxosc_step = 1;
    status_t status = STATUS_SCU_FXOSC_READY_TIMEOUT;
    regValue = SCU->FXOSC_CTRL;
    for(timeoutLoopCnt = 0; timeoutLoopCnt < FEATURE_CLOCK_TIMEOUT_LOOP_CNT; timeoutLoopCnt++)
    {
        /* sweep FXOSC gain */
        SCU_SetFxoscGain(SCU, (uint8_t)fxosc_gain);
        if (fxosc_gain == 1)
        {
            fxosc_step = 1;
        }
        else if (fxosc_gain == 7)
        {
            fxosc_step = -1;
        }
        else
        {
            /* Nothing to do */
        }
        fxosc_gain += fxosc_step;

        if (SCU_GetFXOSCValid(SCU))
        {
            status = STATUS_SUCCESS;
            break;
        }
    }
    /* Update user config gain */
    SCU->FXOSC_CTRL = regValue;
    return status;
}

/*! @} */ /* End of Oscillator Wait Helpers */

/*******************************************************************************
 * IPC Configuration
 ******************************************************************************/
/*!
 * @name IPC Configuration
 * @{
 */

/*!
 * @brief Get the IPC control register pointer for a given clock name.
 *
 * @param[in] clockName  Peripheral clock identifier.
 * @return Pointer to the IPC control register.
 */
static inline volatile uint32_t *CLOCK_GetIpcCtrlReg(clock_names_t clockName)
{
    uint32_t idx = clockNameMappings[clockName];
    return &(IPC->CTRL[idx]);
}

/*!
 * @brief Apply IPC peripheral clock configuration.
 *
 * Iterates through the peripheral clock array and programs each
 * clock gate, source, and divider.
 *
 * @param[in] peripheralClockConfig  Pointer to the IPC configuration.
 */
static void CLOCK_SYS_SetIpcConfiguration(const ipc_config_t * peripheralClockConfig)
{
    DEV_ASSERT(peripheralClockConfig != NULL);
    uint32_t i;
    volatile uint32_t * ipcCtrlReg;

    if ((peripheralClockConfig != NULL) && (peripheralClockConfig->peripheralClocks != NULL))
    {
        for (i = 0U; i < peripheralClockConfig->count; i++)
        {
            ipcCtrlReg = CLOCK_GetIpcCtrlReg(peripheralClockConfig->peripheralClocks[i].clkName);
            /* Disable the peripheral clock before update new clock source */
            IPC_SetClockMode(ipcCtrlReg, false);

            /* Set peripheral clock control */
            IPC_SetPeripheralClockControl(ipcCtrlReg,
                                          peripheralClockConfig->peripheralClocks[i].clkGate,
                                          (uint32_t)peripheralClockConfig->peripheralClocks[i].clkSrc,
                                          (uint32_t)peripheralClockConfig->peripheralClocks[i].divider);
        }
    }
}

/*! @} */ /* End of IPC Configuration */

/*******************************************************************************
 * Module Reset & Module Clock
 ******************************************************************************/
/*!
 * @name Module Reset & Module Clock
 * @{
 */

/*!
 * @brief Assert and de-assert software reset for a peripheral module.
 *
 * @param[in] clockName  Peripheral clock identifier.
 */
void CLOCK_DRV_ResetModule(clock_names_t clockName)
{
    volatile uint32_t *ipcCtrlReg;
    ipcCtrlReg = CLOCK_GetIpcCtrlReg(clockName);
    /* reset module */
    IPC_ResetModule(ipcCtrlReg);
}

/*!
 * @brief Configure a peripheral clock gate, source, and divider.
 *
 * @param[in] clockName  Peripheral clock identifier.
 * @param[in] clockGate  true to enable clock, false to gate.
 * @param[in] clkSrc     Clock source selection value.
 * @param[in] divider    Clock divider value.
 */
void CLOCK_DRV_SetModuleClock(clock_names_t clockName, bool clockGate, uint32_t clkSrc, uint32_t divider)
{
    volatile uint32_t *ipcCtrlReg;
    ipcCtrlReg = CLOCK_GetIpcCtrlReg(clockName);
    IPC_SetPeripheralClockControl(ipcCtrlReg, clockGate, clkSrc, divider);
}

/*! @} */ /* End of Module Reset & Module Clock */

/*******************************************************************************
 * Frequency Query
 ******************************************************************************/
/*!
 * @name Frequency Query
 * @{
 */

#if FEATURE_SCU_SUPPORT_PLL
/*!
 * @brief Get the current PLL output frequency.
 *
 * @param[out] frequency  Pointer to store the PLL frequency in Hz.
 * @return STATUS_SUCCESS if PLL is locked; STATUS_MCU_GATED_OFF if not.
 */
status_t CLOCK_DRV_GetPllFreq(uint32_t * frequency)
{
    status_t status = STATUS_SUCCESS;
    uint32_t refClkFreq;
    /* Check if PLL is locked */
    if (0U == SCU_GetPllLock(SCU))
    {
        *frequency = 0U;
        status = STATUS_MCU_GATED_OFF;
    }
    else
    {
        if (SCU_PLL_REF_FIRC_CLK == SCU_GetPllRefClk(SCU))
        {
            refClkFreq = FEATURE_SCU_FIRC_FREQ;
        }
        else
        {
            /* FXOSC as reference clock */
            refClkFreq = g_fxoscClkFreq;
        }
        /* Fout = (Fref * Ndiv) / ( 2 * Npre) */
        *frequency = ((refClkFreq / SCU_GetPllRefClkDiv(SCU)) * SCU_GetPllFeedBackDiv(SCU)) >> 1U;
    }
    return status;
}
#endif /* FEATURE_SCU_SUPPORT_PLL */

/*!
 * @brief Get the frequency of any clock in the system.
 *
 * Resolves peripheral clocks (via IPC), raw clock sources, and system/bus
 * clocks to their current frequency in Hz.
 *
 * @param[in]  clockName  Clock identifier.
 * @param[out] frequency  Pointer to store the frequency value.
 * @return STATUS_SUCCESS on success; STATUS_MCU_GATED_OFF if clock is gated.
 */
status_t CLOCK_DRV_GetFreq(clock_names_t clockName, uint32_t * frequency)
{
    status_t status = STATUS_SUCCESS;
    volatile uint32_t *ipcCtrlReg;
    uint32_t value;
    peripheral_clock_source_t clockSource;
    if (clockName < IPC_PERI_END_OF_CLOCK)
    {
        /* Note: API won't check if IP have function clock or not */
        ipcCtrlReg = CLOCK_GetIpcCtrlReg(clockName);
        if (0U == (*ipcCtrlReg & IPC_CTRL_CLKEN_MASK))
        {
            /* IP clock is disabled */
            *frequency = 0;
            status = STATUS_MCU_GATED_OFF;
        }
        else
        {
            value = ((*ipcCtrlReg & IPC_CTRL_SRCSEL_MASK) >> IPC_CTRL_SRCSEL_SHIFT);
            clockSource = (peripheral_clock_source_t)value;
            switch(clockSource)
            {
                case CLK_SRC_FIRC:
                    *frequency = FEATURE_SCU_FIRC_FREQ;
                    break;
                case CLK_SRC_SIRC:
                    *frequency = FEATURE_SCU_SIRC_FREQ;
                    break;
                case CLK_SRC_FXOSC:
                    *frequency = g_fxoscClkFreq;
                    break;
    #if defined(FEATURE_SCU_SUPPORT_SXOSC) && (FEATURE_SCU_SUPPORT_SXOSC == 1)
                    case CLK_SRC_SXOSC:
                    *frequency = g_sxoscClkFreq;
                    break;
    #endif
                case CLK_SRC_LPO:
                    *frequency = FEATURE_SCU_LPO_FREQ;
                    break;
                default :
                    *frequency = 0;
                    break;
            }
            /* add divider */
            *frequency = *frequency / (((*ipcCtrlReg & IPC_CTRL_DIV_MASK) >> IPC_CTRL_DIV_SHIFT) + 1U);
        }
    }
    else if (clockName < IPC_END_OF_CLOCK)
    {
        switch (clockName)
        {
            case IPC_SIRC_CLK:
                *frequency = FEATURE_SCU_SIRC_FREQ;
                break;
            case IPC_FIRC_CLK:
                *frequency = FEATURE_SCU_FIRC_FREQ;
                break;
            case IPC_FXOSC_CLK:
                *frequency = g_fxoscClkFreq;
                break;
#if defined(FEATURE_SCU_SUPPORT_SXOSC) && (FEATURE_SCU_SUPPORT_SXOSC == 1)
                case IPC_SXOSC_CLK:
                *frequency = g_sxoscClkFreq;
                break;
#endif
            default :
                *frequency = 0;
                break;
        }
    }
    else if (clockName < CLOCK_NAME_COUNT)
    {
        switch ((scu_system_clock_src_t)SCU_GetSysClkSrc(SCU))
        {
            case SCU_SYSTEM_CLOCK_SRC_FIRC:
                *frequency = FEATURE_SCU_FIRC_FREQ;
                break;
            case SCU_SYSTEM_CLOCK_SRC_FXOSC:
                *frequency = g_fxoscClkFreq;
                break;
            case SCU_SYSTEM_CLOCK_SRC_SIRC:
                *frequency = FEATURE_SCU_SIRC_FREQ;
                break;
            default:
                *frequency = 0;
                break;
        }
        /* get core clock frequency */
        *frequency = *frequency / ((uint32_t)SCU_GetSysClkDiv(SCU) + 1U);
        if (CORE_CLK != clockName)
        {
            /* Fast bus clock */
            *frequency = *frequency / ((uint32_t)SCU_GetFastBusClkDiv(SCU) + 1U);
        }
        if(SLOW_BUS_CLK == clockName)
        {
            /* Slow bus clock */
            *frequency = *frequency / ((uint32_t)SCU_GetSlowBusClkDiv(SCU) + 1U);
        }
    }
    else
    {
        /* Nothing to do */
    }
    return status;
}

/*! @} */ /* End of Frequency Query */

/*******************************************************************************
 * Clock Manager Framework
 ******************************************************************************/
/*!
 * @name Clock Manager Framework
 * @{
 */

/*!
 * @brief Install clock configuration table and callback array.
 *
 * @param[in] clockConfigsPtr  Pointer to the clock configuration table.
 * @param[in] configsNumber    Number of configurations.
 * @param[in] callbacksPtr     Pointer to the callback table.
 * @param[in] callbacksNumber  Number of callbacks.
 * @return STATUS_SUCCESS always.
 */
status_t CLOCK_SYS_Init(clock_manager_user_config_t const **clockConfigsPtr,
                        uint8_t configsNumber,
                        clock_manager_callback_user_config_t **callbacksPtr,
                        uint8_t callbacksNumber)
{
    DEV_ASSERT(clockConfigsPtr != NULL);

    g_clockState.configTable     = clockConfigsPtr;
    g_clockState.clockConfigNum  = configsNumber;
    g_clockState.callbackConfig  = callbacksPtr;
    g_clockState.callbackNum     = callbacksNumber;

    /*
     * errorCallbackIndex is the index of the callback which returns error
     * during clock mode switch. If all callbacks return success, then the
     * errorCallbackIndex is callbacksNumber.
     */
    g_clockState.errorCallbackIndex = callbacksNumber;

    return STATUS_SUCCESS;
}

/*!
 * @brief Switch to a pre-defined clock configuration with notification.
 *
 * Sends BEFORE/AFTER notifications to registered callbacks and switches
 * to the target configuration. Sends RECOVER on failure.
 *
 * @param[in] targetConfigIndex  Index of the target clock configuration.
 * @param[in] policy             Transition policy (graceful or forceful).
 * @return STATUS_SUCCESS on success; error code otherwise.
 */
status_t CLOCK_SYS_UpdateConfiguration(uint8_t targetConfigIndex,
                                       clock_manager_policy_t policy)
{
    uint8_t callbackIdx;
    bool successfulSetConfig;           /* Set Configuration status */
    status_t ret = STATUS_SUCCESS;
    const clock_manager_callback_user_config_t * callbackConfig;
    clock_notify_struct_t notifyStruct;

    DEV_ASSERT(targetConfigIndex < g_clockState.clockConfigNum);       /* Clock configuration index is out of range. */

    notifyStruct.targetClockConfigIndex = targetConfigIndex;
    notifyStruct.policy                 = policy;

    /* Disable interrupts */
    INT_SYS_DisableIRQGlobal();
    /* Set error callback index as callbackNum, which means no callback error now.*/
    g_clockState.errorCallbackIndex = g_clockState.callbackNum;

    /* First step: Send "BEFORE" notification. */
    notifyStruct.notifyType = CLOCK_MANAGER_NOTIFY_BEFORE;

    /* Send notification to all callback. */
    for (callbackIdx=0; callbackIdx<g_clockState.callbackNum; callbackIdx++)
    {
        callbackConfig = g_clockState.callbackConfig[callbackIdx];
        if ((callbackConfig != NULL) &&
            (callbackConfig->callbackType != CLOCK_MANAGER_CALLBACK_AFTER))
        {
            if (STATUS_SUCCESS !=
                (*callbackConfig->callback)(&notifyStruct,
                                            callbackConfig->callbackData))
            {
                g_clockState.errorCallbackIndex = callbackIdx;

                if (CLOCK_MANAGER_POLICY_AGREEMENT == policy)
                {
                    /* Save the error callback index. */
                    ret = STATUS_MCU_NOTIFY_BEFORE_ERROR;
                    break;
                }
            }
        }
    }

    /* If all callback success or forceful policy is used. */
    if ((STATUS_SUCCESS == ret) ||
        (CLOCK_MANAGER_POLICY_FORCIBLE == policy)) /*PRQA S 2996*/
    {
        /* clock mode switch. */
        ret = CLOCK_SYS_SetConfiguration(g_clockState.configTable[targetConfigIndex]);
        successfulSetConfig = (STATUS_SUCCESS == ret);

        g_clockState.curConfigIndex = targetConfigIndex;
    }
    else
    {
        /* Unsuccessful setConfiguration */
        successfulSetConfig = false;
    }

    if(successfulSetConfig){
        notifyStruct.notifyType = CLOCK_MANAGER_NOTIFY_AFTER;

        for (callbackIdx=0; callbackIdx<g_clockState.callbackNum; callbackIdx++)
        {
            callbackConfig = g_clockState.callbackConfig[callbackIdx];
            if ((callbackConfig != NULL) &&
                (callbackConfig->callbackType != CLOCK_MANAGER_CALLBACK_BEFORE))
            {
                if (STATUS_SUCCESS !=
                    (*callbackConfig->callback)(&notifyStruct,
                                                callbackConfig->callbackData))
                {
                    g_clockState.errorCallbackIndex = callbackIdx;

                    if (CLOCK_MANAGER_POLICY_AGREEMENT == policy)
                    {
                        /* Save the error callback index. */
                        ret = STATUS_MCU_NOTIFY_AFTER_ERROR;
                        break;
                    }
                }
            }
        }
    }
    else /* Error occurs, need to send "RECOVER" notification. */
    {
        notifyStruct.notifyType = CLOCK_MANAGER_NOTIFY_RECOVER;
        for (callbackIdx=0; callbackIdx<g_clockState.callbackNum; callbackIdx++)
        {
            callbackConfig = g_clockState.callbackConfig[callbackIdx];
            if (callbackConfig != NULL)
            {
                (void)(*callbackConfig->callback)(&notifyStruct,
                                                  callbackConfig->callbackData);
            }
        }
    }

    /* Enable interrupts */
    INT_SYS_EnableIRQGlobal();

    return ret;
}

/*!
 * @brief Get the index of the currently active clock configuration.
 *
 * @return Current clock configuration index.
 */
uint8_t CLOCK_SYS_GetCurrentConfiguration(void)
{
    return g_clockState.curConfigIndex;
}

/*!
 * @brief Get the callback that returned error during the last clock switch.
 *
 * @return Pointer to the error callback configuration, or NULL if all succeeded.
 */
clock_manager_callback_user_config_t* CLOCK_SYS_GetErrorCallback(void)
{
    clock_manager_callback_user_config_t *retValue;

    /* If all callbacks return success. */
    if (g_clockState.errorCallbackIndex >= g_clockState.clockConfigNum)
    {
        retValue = NULL;
    }
    else
    {
        retValue = g_clockState.callbackConfig[g_clockState.errorCallbackIndex];
    }
    return retValue;
}

/*!
 * @brief Get clock frequency (legacy wrapper over CLOCK_DRV_GetFreq).
 *
 * @param[in]  clockName  Clock identifier.
 * @param[out] frequency  Pointer to store the frequency in Hertz.
 * @return STATUS_SUCCESS on success; error code otherwise.
 */
status_t CLOCK_SYS_GetFreq(clock_names_t clockName, uint32_t *frequency)
{
    return CLOCK_DRV_GetFreq(clockName,frequency);
}

/*!
 * @brief Apply a clock configuration (legacy wrapper over CLOCK_DRV_Init).
 *
 * @param[in] config  Pointer to the target clock configuration.
 * @return STATUS_SUCCESS on success; error code otherwise.
 */
status_t CLOCK_SYS_SetConfiguration(clock_manager_user_config_t const * config)
{
    return CLOCK_DRV_Init(config);
}

/*! @} */ /* End of Clock Manager Framework */

/*******************************************************************************
 * EOF
 ******************************************************************************/

/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file clock_YTM32B1Lx.h
 * @version 1.5.0
 *
 * @brief Clock Manager - YTM32B1Lx device-specific type definitions and API declarations.
 *
 * This header defines all configuration structures, enumerations, and function
 * prototypes for the Clock Manager on the YTM32B1Lx device family. It covers
 * SCU system clock configuration, external oscillator (FXOSC/SXOSC) settings,
 * CMU (Clock Monitor Unit) thresholds, IPC peripheral clock configuration,
 * and the Clock Manager notification framework.
 *
 * Contents are organized into the following groups:
 *   - **Compilation Switches** - CMU default/always-on macros.
 *   - **External Declarations** - Peripheral features list and crystal frequency.
 *   - **SCU Types** - Clock source enumerations, clock output, FXOSC/SXOSC
 *     configuration structures, SCU master configuration.
 *   - **CMU Types** - Reference clock enumeration, channel and top-level config.
 *   - **Clock Divider Types** - System divider and IPC divider enumerations.
 *   - **IPC Types** - Peripheral clock configuration and IPC master config.
 *   - **Clock Manager Types** - User configuration, notification framework,
 *     callback types, and Clock Manager internal state.
 *   - **API Declarations** - Public function prototypes.
 */

#ifndef CLOCK_YTM32B1Lx_H
#define CLOCK_YTM32B1Lx_H

#include "device_registers.h"
#include "status.h"

/*******************************************************************************
 * Compilation Switches
 ******************************************************************************/
/*!
 * @name Compilation Switches
 * @brief CMU default-on / default-off compile-time selection.
 * @{
 */
#if !(defined(CMU_DEFAULT_OFF))
#define CMU_ALWAYS_ON
#endif /* CMU_DEFAULT_OFF */
/*! @} */

/*******************************************************************************
 * External Declarations
 ******************************************************************************/
/*!
 * @name External Declarations
 * @brief Peripheral features list and reference crystal frequency.
 * @{
 */

/*! @brief Peripheral features list - per-clock-name feature flags. */
extern const uint8_t peripheralFeaturesList[CLOCK_NAME_COUNT];

/*! @brief EXTAL0 clock frequency (Hz). */
extern uint32_t g_xtal0ClkFreq;

/*! @brief The maximum number of system clock dividers. */
#define SYS_CLK_MAX_NO    3U
/*! @brief Index for the system (core) clock divider. */
#define SYSTEM_CLK_INDEX  0U
/*! @brief Index for the bus clock divider. */
#define BUS_CLK_INDEX     1U

/*! @} */

/*******************************************************************************
 * SCU Types
 ******************************************************************************/
/*!
 * @name SCU Types
 * @brief SCU clock source, oscillator, and master configuration types.
 * @{
 */

/*!
 * @brief SCU CLKOUT source selection.
 */
typedef enum
{
    SCU_CLKOUT_SEL_DISABLE      = 0U,     /*!< SCU CLKOUT Disable           */
    SCU_CLKOUT_SEL_FIRC_CLK     = 1U,     /*!< SCU CLKOUT Select FIRC CLK   */
    SCU_CLKOUT_SEL_SIRC_CLK     = 2U,     /*!< SCU CLKOUT Select SIRC CLK   */
    SCU_CLKOUT_SEL_FXOSC_CLK    = 3U,     /*!< SCU CLKOUT Select FXOSC CLK  */
    SCU_CLKOUT_SEL_SXOSC_CLK    = 4U,     /*!< SCU CLKOUT Select SXOSC CLK  */
    SCU_CLKOUT_SEL_LPO_CLK      = 5U,     /*!< SCU CLKOUT Select LPO CLK    */
    SCU_CLKOUT_SEL_CORE_CLK     = 7U,     /*!< SCU CLKOUT Select CORE CLK   */
} scu_clkout_src_t;

/*!
 * @brief SCU clock-out configuration structure.
 */
typedef struct
{
    bool              enable;           /*!< SCU ClockOut enable or not.            */
    uint8_t           divider;          /*!< SCU ClockOut divider                   */
    scu_clkout_src_t  source;           /*!< SCU ClockOut source select.            */
} scu_clock_out_config_t;

/*!
 * @brief SCU FXOSC (fast external oscillator) configuration.
 */
typedef struct
{
    bool enable;                       /*!< FXOSC enable */
    bool bypassMode;                   /*!< FXOSC bypass mode*/
    uint8_t gainSelection;             /*!< FXOSC gain selection */
    uint32_t frequency;                /*!< FXOSC frequency */
    bool autoGainDisable;              /*!< FXOSC automatic gain control disable (ALCDIS). false to use default (auto gain enabled). */
} scu_fxosc_config_t;

/*!
 * @brief SCU SXOSC (slow external oscillator) configuration.
 */
typedef struct
{
    bool enable;                       /*!< SXOSC enable */
    uint8_t gainSelection;             /*!< SXOSC gain selection */
    bool bypassMode;                   /*!< SXOSC bypass mode*/
    bool deepsleepEnable;              /*!< SXOSC deepsleep enable */
    bool standbyEnable;                /*!< SXOSC standby enable */
    uint32_t frequency;                /*!< SXOSC frequency */
} scu_sxosc_config_t;

/*!
 * @brief SCU system clock source selection.
 */
typedef enum {
    SCU_SYSTEM_CLOCK_SRC_FIRC     = 0U,       /*!< Fast IRC         */
    SCU_SYSTEM_CLOCK_SRC_FXOSC    = 2U,       /*!< External OSC     */
    SCU_SYSTEM_CLOCK_SRC_SIRC     = 3U,       /*!< Slow IRC         */
    SCU_SYSTEM_CLOCK_SRC_NONE     = 255U      /*!< MAX value        */
} scu_system_clock_src_t;

/*!
 * @brief SCU master configuration structure.
 */
typedef struct
{
    bool                      sircDeepSleepEnable;
    bool                      sircStandbyEnable;
    bool                      fircEnable;
    bool                      fircDeepSleepEnable;
    scu_system_clock_src_t    sysClkSrc;                      /*!< Core clock selection */
    scu_fxosc_config_t        fxoscConfig;                    /*!< External fast oscillator configuration.           */
    scu_sxosc_config_t        sxoscConfig;                    /*!< External slow oscillator configuration.           */
    uint8_t                   sysDiv;                         /*!< system clock divider */
    uint8_t                   fastBusDiv;                     /*!< IPS Fast Bus clock divider */
    uint8_t                   slowBusDiv;                     /*!< IPS Slow Bus clock divider */
    uint8_t                   flashDiv;                       /*!< Flash Clock divider */

#if defined(FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT) && (FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT == 0)
    scu_clock_out_config_t    clockOutConfig;                 /*!< Clock Out configuration. */
#endif /* FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT */
} scu_config_t;

/*! @} */ /* End of SCU Types */

/*******************************************************************************
 * CMU Types
 ******************************************************************************/
/*!
 * @name CMU Types
 * @brief Clock Monitor Unit reference clock and channel configuration types.
 * @{
 */

/*!
 * @brief CMU reference clock source selection.
 */
typedef enum
{
    CMU_REF_SIRC_CLOCK  = 0, /*!< Use SIRC as CMU clock reference clock source. */
    CMU_REF_FXOSC_CLOCK = 1, /*!< Use FXOSC as CMU clock reference clock source. */
} cmu_refclk_t;

/*!
 * @brief CMU individual channel configuration.
 */
typedef struct
{
    bool enable;                 /*!< Specifies whether the channel is enabled or disabled */
    bool resetEnable;            /*!< Specifies whether the channel clock error reset enable */
#if defined(FEATURE_SCU_SUPPORT_CMU_REFCLK_SEL) && (FEATURE_SCU_SUPPORT_CMU_REFCLK_SEL)
    cmu_refclk_t refClock;       /*!< Specifies the reference clock source for the channel */
#endif
    uint16_t compareHigh;        /*!< CMU counter compare high threshold */
    uint16_t compareLow;         /*!< CMU counter compare low threshold */
} cmu_ch_config_t;

/*!
 * @brief CMU top-level configuration - aggregates all monitored channels.
 */
typedef struct
{
    cmu_ch_config_t fircClockMonitor;           /*!< Clock monitor for firc clock */
    cmu_ch_config_t fxoscClockMonitor;          /*!< Clock monitor for fxosc clock */
} cmu_config_t;

/*! @} */ /* End of CMU Types */

/*******************************************************************************
 * Clock Divider Types
 ******************************************************************************/
/*!
 * @name Clock Divider Types
 * @brief System and IPC clock divider enumerations.
 * @{
 */

/*!
 * @brief SCU system clock divider values.
 */
typedef enum {
    SCU_SYS_CLK_DIV_BY_1  = 0,     /*!< Divided by 1 */
    SCU_SYS_CLK_DIV_BY_2  = 1,     /*!< Divided by 2 */
    SCU_SYS_CLK_DIV_BY_3  = 2,     /*!< Divided by 3 */
    SCU_SYS_CLK_DIV_BY_4  = 3,     /*!< Divided by 4 */
    SCU_SYS_CLK_DIV_BY_5  = 4,     /*!< Divided by 5 */
    SCU_SYS_CLK_DIV_BY_6  = 5,     /*!< Divided by 6 */
    SCU_SYS_CLK_DIV_BY_7  = 6,     /*!< Divided by 7 */
    SCU_SYS_CLK_DIV_BY_8  = 7,     /*!< Divided by 8 */
    SCU_SYS_CLK_DIV_BY_9  = 8,     /*!< Divided by 9 */
    SCU_SYS_CLK_DIV_BY_10 = 9,     /*!< Divided by 10 */
    SCU_SYS_CLK_DIV_BY_11 = 10,    /*!< Divided by 11 */
    SCU_SYS_CLK_DIV_BY_12 = 11,    /*!< Divided by 12 */
    SCU_SYS_CLK_DIV_BY_13 = 12,    /*!< Divided by 13 */
    SCU_SYS_CLK_DIV_BY_14 = 13,    /*!< Divided by 14 */
    SCU_SYS_CLK_DIV_BY_15 = 14,    /*!< Divided by 15 */
    SCU_SYS_CLK_DIV_BY_16 = 15,    /*!< Divided by 16 */
} system_clock_divider_t;

/*!
 * @brief SCU IPC peripheral clock divider values.
 */
typedef enum 
{
    SCU_IPC_CLK_DIV_BY_1  = 0,     /*!< Divider by 1 */   
    SCU_IPC_CLK_DIV_BY_2  = 1,     /*!< Divider by 2 */  
    SCU_IPC_CLK_DIV_BY_3  = 2,     /*!< Divider by 3 */  
    SCU_IPC_CLK_DIV_BY_4  = 3,     /*!< Divider by 4 */ 
    SCU_IPC_CLK_DIV_BY_5  = 4,     /*!< Divided by 5 */
    SCU_IPC_CLK_DIV_BY_6  = 5,     /*!< Divided by 6 */
    SCU_IPC_CLK_DIV_BY_7  = 6,     /*!< Divided by 7 */
    SCU_IPC_CLK_DIV_BY_8  = 7,     /*!< Divided by 8 */
    SCU_IPC_CLK_DIV_BY_9  = 8,     /*!< Divided by 9 */
    SCU_IPC_CLK_DIV_BY_10 = 9,     /*!< Divided by 10 */
    SCU_IPC_CLK_DIV_BY_11 = 10,    /*!< Divided by 11 */
    SCU_IPC_CLK_DIV_BY_12 = 11,    /*!< Divided by 12 */
    SCU_IPC_CLK_DIV_BY_13 = 12,    /*!< Divided by 13 */
    SCU_IPC_CLK_DIV_BY_14 = 13,    /*!< Divided by 14 */
    SCU_IPC_CLK_DIV_BY_15 = 14,    /*!< Divided by 15 */
    SCU_IPC_CLK_DIV_BY_16 = 15,    /*!< Divided by 16 */
} ipc_clock_divider_t;

/*!
 * @brief IPC peripheral clock divider generic values.
 */
typedef enum
{
    DIV_BY_1  =  0U,        /*!< Divided by 1      */
    DIV_BY_2  =  1U,        /*!< Divided by 2      */
    DIV_BY_3  =  2U,        /*!< Divided by 3      */
    DIV_BY_4  =  3U,        /*!< Divided by 4      */
    DIV_BY_5  =  4U,        /*!< Divided by 5      */
    DIV_BY_6  =  5U,        /*!< Divided by 6      */
    DIV_BY_7  =  6U,        /*!< Divided by 7      */
    DIV_BY_8  =  7U,        /*!< Divided by 8      */
    DIV_BY_9  =  8U,        /*!< Divided by 9      */
    DIV_BY_10 =  9U,        /*!< Divided by 10     */
    DIV_BY_11 = 10U,        /*!< Divided by 11     */
    DIV_BY_12 = 11U,        /*!< Divided by 12     */
    DIV_BY_13 = 12U,        /*!< Divided by 13     */
    DIV_BY_14 = 13U,        /*!< Divided by 14     */
    DIV_BY_15 = 14U,        /*!< Divided by 15     */
    DIV_BY_16 = 15U         /*!< Divided by 16     */
} peripheral_clock_divider_t;

/*! @} */ /* End of Clock Divider Types */

/*******************************************************************************
 * IPC Types
 ******************************************************************************/
/*!
 * @name IPC Types
 * @brief Peripheral clock configuration and IPC master configuration types.
 * @{
 */

/*!
 * @brief IPC peripheral clock instance configuration.
 */
typedef struct
{
    /* clockName   is the name of the peripheral clock
     *    must be one of the following values (see the clock_names_t type from YTM32B1Lxx_features.h)
     *    IPC_DMA_CLOCK
     *    IPC_GPIO_CLOCK
     *    ...
     *    IPC_SPI0_CLOCK
     */
    clock_names_t clkName;
    bool clkGate;                                      /*!< Peripheral clock gate.                     */
    peripheral_clock_source_t clkSrc;                  /*!< Peripheral clock source.                   */
    peripheral_clock_divider_t divider;                /*!< Peripheral clock divider value.            */
} peripheral_clock_config_t;

/*!
 * @brief IPC master configuration - peripheral clock array.
 */
typedef struct
{
    uint32_t count;                                    /*!< Number of peripherals to be configured.               */
    peripheral_clock_config_t const * peripheralClocks;       /*!< Pointer to the peripheral clock configurations array. */
} ipc_config_t;

/*! @} */ /* End of IPC Types */

/*******************************************************************************
 * Clock Manager Types
 ******************************************************************************/
/*!
 * @name Clock Manager Types
 * @brief Top-level user configuration, notification framework, and internal state.
 * @{
 */

/*!
 * @brief Clock manager user configuration - aggregates SCU, IPC, and CMU settings.
 */
typedef struct
{
    ipc_config_t                 ipcConfig;       /*!< IPC Clock configuration.      */
    scu_config_t const           *scuConfigPtr;   /*!< SCU Clock configuration.      */
    cmu_config_t const           *cmuConfigPtr;   /*!< SCU CMU Clock configuration.      */
} clock_manager_user_config_t;

/*! @brief Alias for backward compatibility. */
typedef clock_manager_user_config_t clock_user_config_t;

/*!
 * @brief System clock configuration (source + dividers).
 */
typedef struct
{
    clock_names_t src;                         /*!< System clock source. */
    uint16_t dividers[SYS_CLK_MAX_NO];         /*!< System clock dividers. Value by which system clock is divided. 0 means that system clock is not divided. */
} sys_clk_config_t;

/*!
 * @brief Clock source configuration (for external oscillator inputs).
 */
typedef struct
{
	bool       enable;                   /*!< Enable/disable clock source. */
    uint32_t   refFreq;                  /*!< Frequency of the input reference clock. It applies to external oscillator clock sources */
	uint16_t   outputDiv1;               /*!< First output divider. It's used as protocol clock by modules. Zero means that divider is disabled.   /
										  *   Possible values 0(disabled), 1, 2, 4, 8, 16, 32, 64; all the other values are not valid.             /
										  */
	uint16_t   outputDiv2;               /*!< Second output divider. It's used as protocol clock by modules. Zero means that divider is disabled.   /
										  *   Possible values 0(disabled), 1, 2, 4, 8, 16, 32, 64; all the other values are not valid.              /
										  */

} clock_source_config_t;

/*!
 * @brief Clock transition notification type.
 */
typedef enum
{
    CLOCK_MANAGER_NOTIFY_RECOVER = 0x00U,  /*!< Notify IP to recover to previous work state.      */
    CLOCK_MANAGER_NOTIFY_BEFORE  = 0x01U,  /*!< Notify IP that system will change clock setting.  */
    CLOCK_MANAGER_NOTIFY_AFTER   = 0x02U,  /*!< Notify IP that have changed to new clock setting. */
} clock_manager_notify_t;

/*!
 * @brief Callback type - specifies which notifications the callback handles.
 */
typedef enum
{
    CLOCK_MANAGER_CALLBACK_BEFORE       = 0x01U, /*!< Callback handles BEFORE notification.          */
    CLOCK_MANAGER_CALLBACK_AFTER        = 0x02U, /*!< Callback handles AFTER notification.           */
    CLOCK_MANAGER_CALLBACK_BEFORE_AFTER = 0x03U  /*!< Callback handles BEFORE and AFTER notification */
} clock_manager_callback_type_t;

/*!
 * @brief Clock transition policy - graceful or forceful.
 */
typedef enum
{
    CLOCK_MANAGER_POLICY_AGREEMENT,  /*!< Clock transfers gracefully. */
    CLOCK_MANAGER_POLICY_FORCIBLE    /*!< Clock transfers forcefully. */
} clock_manager_policy_t;

/*!
 * @brief Clock notification structure passed to callback functions.
 */
typedef struct
{
    uint8_t targetClockConfigIndex;    /*!< Target clock configuration index. */
    clock_manager_policy_t policy;     /*!< Clock transition policy.          */
    clock_manager_notify_t notifyType; /*!< Clock notification type.          */
} clock_notify_struct_t;

/*!
 * @brief Clock manager callback function pointer type.
 */
typedef status_t (*clock_manager_callback_t)(clock_notify_struct_t *notify,
                                             void* callbackData);

/*!
 * @brief Clock manager callback configuration - function, type, and user data.
 */
typedef struct
{
    clock_manager_callback_t      callback;      /*!< Entry of callback function.     */
    clock_manager_callback_type_t callbackType;  /*!< Callback type.                  */
    void* callbackData;                          /*!< Parameter of callback function. */
} clock_manager_callback_user_config_t;

/*!
 * @brief Clock manager internal state.
 */
typedef struct
{
    clock_manager_user_config_t const **configTable;/*!< Pointer to clock configure table.*/
    uint8_t clockConfigNum;                         /*!< Number of clock configurations.  */
    uint8_t curConfigIndex;                         /*!< Index of current configuration.  */
    clock_manager_callback_user_config_t **callbackConfig; /*!< Pointer to callback table.*/
    uint8_t callbackNum;                            /*!< Number of clock callbacks.       */
    uint8_t errorCallbackIndex;                     /*!< Index of callback returns error. */
} clock_manager_state_t;

/*! @} */ /* End of Clock Manager Types */

/*******************************************************************************
 * API Declarations
 ******************************************************************************/
/*!
 * @name API Declarations
 * @brief Clock Manager public function prototypes.
 * @{
 */

#if defined (__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*!
 * @brief Assert and de-assert software reset for a peripheral module.
 *
 * @param[in] clockName  Peripheral clock identifier.
 */
void CLOCK_DRV_ResetModule(clock_names_t clockName);

/*!
 * @brief Configure a peripheral clock gate, source, and divider.
 *
 * @param[in] clockName  Peripheral clock identifier.
 * @param[in] clockGate  true to enable the clock, false to gate.
 * @param[in] clkSrc     Clock source selection value.
 * @param[in] divider    Clock divider value.
 */
void CLOCK_DRV_SetModuleClock(clock_names_t clockName, bool clockGate, uint32_t clkSrc, uint32_t divider);

/*!
 * @brief Install pre-defined clock configurations and callback table.
 *
 * @param[in] clockConfigsPtr  Pointer to the clock configuration table.
 * @param[in] configsNumber    Number of clock configurations in the table.
 * @param[in] callbacksPtr     Pointer to the callback configuration table.
 * @param[in] callbacksNumber  Number of callback configurations in the table.
 * @return STATUS_SUCCESS on success; error code otherwise.
 */
status_t CLOCK_SYS_Init(clock_manager_user_config_t const **clockConfigsPtr,
                        uint8_t configsNumber,
                        clock_manager_callback_user_config_t **callbacksPtr,
                        uint8_t callbacksNumber);

/*!
 * @brief Switch to a pre-defined clock configuration by index.
 *
 * @param[in] targetConfigIndex  Index of the target clock configuration.
 * @param[in] policy             Transition policy (graceful or forceful).
 * @return STATUS_SUCCESS on success; error code otherwise.
 */
status_t CLOCK_SYS_UpdateConfiguration(uint8_t targetConfigIndex,
                                       clock_manager_policy_t policy);

/*!
 * @brief Apply a clock configuration directly (legacy wrapper over CLOCK_DRV_Init).
 *
 * @param[in] config  Pointer to the target clock configuration.
 * @return STATUS_SUCCESS on success; error code otherwise.
 *
 * @note This function should be called only in run mode.
 */
status_t CLOCK_SYS_SetConfiguration(clock_manager_user_config_t const * config);

/*!
 * @brief Get the index of the currently active clock configuration.
 *
 * @return Current clock configuration index.
 */
uint8_t CLOCK_SYS_GetCurrentConfiguration(void);

/*!
 * @brief Get the callback that returned error during the last clock switch.
 *
 * @return Pointer to the error callback configuration, or NULL if all succeeded.
 */
clock_manager_callback_user_config_t* CLOCK_SYS_GetErrorCallback(void);

/*!
 * @brief Get clock frequency (legacy wrapper over CLOCK_DRV_GetFreq).
 *
 * @param[in]  clockName  Clock identifier.
 * @param[out] frequency  Pointer to store the frequency value in Hertz.
 * @return STATUS_SUCCESS on success; error code otherwise.
 */
status_t CLOCK_SYS_GetFreq(clock_names_t clockName, uint32_t *frequency);

/*!
 * @brief Wait for FXOSC to become valid.
 *
 * @return STATUS_SUCCESS if FXOSC is valid; error code on timeout.
 */
status_t CLOCK_SYS_WaitFXOSCValid(void);

/*! @} */ /* End of API Declarations */

#if defined (__cplusplus)
}
#endif /* __cplusplus*/

#endif /* CLOCK_YTM32B1Lx_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

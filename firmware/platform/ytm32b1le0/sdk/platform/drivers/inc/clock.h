/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file clock.h
 * @version 1.5.0
 *
 * @brief Clock Driver - Common public API declarations.
 *
 * This header dispatches to the correct device-specific clock implementation
 * based on the CPU macro, and declares the two core driver-layer functions
 * shared by all device families:
 *   - CLOCK_DRV_Init() - Apply a clock configuration.
 *   - CLOCK_DRV_GetFreq() - Query a named clock frequency.
 *
 * @note Include clock_manager.h (which includes this file) from application code.
 */

#ifndef CLOCK_H
#define CLOCK_H

#include "device_registers.h"
#include "status.h"

/*
 * Include the cpu specific clock API header files.
 */

#if (defined(CPU_YTM32B1LD0))
    /* YTM32B1LD0 Clock System Level API header file */
    #include "../src/clock/YTM32B1LD0/clock_YTM32B1LD0.h"
#elif (defined(YTM32B1L_SERIES))
    /* YTM32B1Lx Clock System Level API header file */
    #include "../src/clock/YTM32B1Lx/clock_YTM32B1Lx.h"
#elif (defined(CPU_YTM32B1MD0))
    /* YTM32B1MD0 Clock System Level API header file */
    #include "../src/clock/YTM32B1MD0/clock_YTM32B1MD0.h"
#elif (defined(YTM32B1M_SERIES))
    /* YTM32B1Mx Clock System Level API header file */
    #include "../src/clock/YTM32B1Mx/clock_YTM32B1Mx.h"
#elif (defined(YTM32B1H_SERIES))
    /* YTM32B1Hx Clock System Level API header file */
    #include "../src/clock/YTM32B1Hx/clock_YTM32B1Hx.h"
#elif (defined(YTM32Z1M_SERIES))
    /* YTM32Z1x Clock System Level API header file */
    #include "../src/clock/YTM32Z1x/clock_YTM32Z1x.h"
#elif (defined(YTM32Z1DS_SERIES))
    /* YTM32Z1DSx Clock System Level API header file */
    #include "../src/clock/YTM32Z1DSx/clock_YTM32Z1DSx.h"
#else
    #error "No valid CPU defined!"
#endif

/*!
 * @addtogroup clock_manager
 */
/*! @{*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*******************************************************************************
 * Initialization & Frequency Query
 ******************************************************************************/
/*!
 * @name Initialization & Frequency Query
 * @brief Core driver-layer functions common to all device families.
 * @{
 */

/*!
 * @brief Get the clock frequency for a named clock.
 *
 * Reads the current hardware configuration and computes the frequency
 * (in Hz) for the clock identified by @a clockName. Supported names
 * are defined in the device-specific clock_names_t enumeration.
 *
 * @param[in]  clockName  Clock identifier (from clock_names_t).
 * @param[out] frequency  Pointer to receive the frequency in Hz.
 *                        Set to 0 if the clock is gated or unsupported.
 * @return Execution status.
 * @retval STATUS_SUCCESS       Frequency retrieved successfully.
 * @retval STATUS_UNSUPPORTED   Clock name not supported on this device.
 * @retval STATUS_MCU_GATED_OFF Peripheral clock is gated (disabled).
 *
 * @pre The clock module must be initialized via CLOCK_DRV_Init().
 */
status_t CLOCK_DRV_GetFreq(clock_names_t clockName, uint32_t *frequency);


/*!
 * @brief Initialize clocks from a user configuration structure.
 *
 * Configures the system clock source, dividers, peripheral clocks, and
 * optional CMU monitoring based on the provided configuration. This is
 * the primary entry point for clock initialization at startup and for
 * runtime reconfiguration.
 *
 * @param[in] config  Pointer to the clock configuration structure.
 *                    Must not be NULL.
 * @return Execution status.
 * @retval STATUS_SUCCESS                 Clock initialization completed.
 * @retval STATUS_SCU_FXOSC_READY_TIMEOUT FXOSC did not start within timeout.
 * @retval STATUS_SCU_PLL_LOCK_TIMEOUT    PLL did not lock within timeout.
 * @retval STATUS_SCU_CLK_SWITCH_TIMEOUT  System clock source switch timed out.
 *
 * @pre  Peripheral clocks used by the clock module itself must be available.
 * @post The system clock tree is configured per @a config. SystemCoreClock
 *       is updated with the new core frequency.
 *
 * @note If an external oscillator is used, ensure the crystal or clock
 *       input is properly connected before calling this function.
 */
status_t CLOCK_DRV_Init(clock_user_config_t const *config);

/*! @} */ /* End of Initialization & Frequency Query */


#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*! @}*/

#endif /* CLOCK_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file device_registers.h
 * @version 1.4.1
 * @brief Shared SoC header-selection entry point for platform/devices.
 */

#ifndef DEVICE_REGISTERS_H
#define DEVICE_REGISTERS_H

/*!
 * @addtogroup devices_device_registers
 * @brief CPU-symbol based selection of SoC register and feature headers.
 * @details
 * Include this header after defining one supported `CPU_<SoC>` symbol. The
 * file pulls in the matching register layout header, feature macro header, and
 * startup declarations required by the shared platform code.
 * @{
 */

/*!
 * @name Common Switch Macros
 * @brief Shared enable/disable values reused by multiple modules.
 * @{
 */

/*! @brief Boolean-like value that represents an enabled configuration. */
#define STD_ON  (1)
/*! @brief Boolean-like value that represents a disabled configuration. */
#define STD_OFF (0)

/*! @} */ /* End of Common Switch Macros */

/*!
 * @brief Include the SoC-specific header set selected by the active CPU symbol.
 *
 * @warning If no supported `CPU_<SoC>` symbol is defined, compilation stops
 *          with `#error "No valid CPU defined!"`.
 */
#if defined(CPU_YTM32B1LE0)
    #define YTM32B1L_SERIES
    #include "YTM32B1LE0/include/YTM32B1LE0.h"
    #include "YTM32B1LE0/include/YTM32B1LE0_features.h"
    #include "YTM32B1LE0/startup/system_YTM32B1LE0.h"
#elif defined(CPU_YTM32B1LE1)
    #define YTM32B1L_SERIES
    #include "YTM32B1LE1/include/YTM32B1LE1.h"
    #include "YTM32B1LE1/include/YTM32B1LE1_features.h"
    #include "YTM32B1LE1/startup/system_YTM32B1LE1.h"
#elif defined(CPU_YTM32B1ME0)
    #define YTM32B1M_SERIES
    #include "YTM32B1ME0/include/YTM32B1ME0.h"
    #include "YTM32B1ME0/include/YTM32B1ME0_features.h"
    #include "YTM32B1ME0/startup/system_YTM32B1ME0.h"
#elif defined(CPU_YTM32B1MD1)
    #define YTM32B1M_SERIES
    #include "YTM32B1MD1/include/YTM32B1MD1.h"
    #include "YTM32B1MD1/include/YTM32B1MD1_features.h"
    #include "YTM32B1MD1/startup/system_YTM32B1MD1.h"
#elif defined(CPU_YTM32B1MD2)
    #define YTM32B1M_SERIES
    #include "YTM32B1MD2/include/YTM32B1MD2.h"
    #include "YTM32B1MD2/include/YTM32B1MD2_features.h"
    #include "YTM32B1MD2/startup/system_YTM32B1MD2.h"
#elif defined(CPU_YTM32B1MC0)
    #define YTM32B1M_SERIES
    #include "YTM32B1MC0/include/YTM32B1MC0.h"
    #include "YTM32B1MC0/include/YTM32B1MC0_features.h"
    #include "YTM32B1MC0/startup/system_YTM32B1MC0.h"
#elif defined(CPU_YTM32B1ME1)
    #define YTM32B1M_SERIES
    #include "YTM32B1ME1/include/YTM32B1ME1.h"
    #include "YTM32B1ME1/include/YTM32B1ME1_features.h"
    #include "YTM32B1ME1/startup/system_YTM32B1ME1.h"
#elif defined(CPU_YTM32B1MF0)
    #define YTM32B1M_SERIES
    #include "YTM32B1MF0/include/YTM32B1MF0.h"
    #include "YTM32B1MF0/include/YTM32B1MF0_features.h"
    #include "YTM32B1MF0/startup/system_YTM32B1MF0.h"
#elif defined(CPU_YTM32B1HA0)
    #define YTM32B1H_SERIES
    #include "YTM32B1HA0/include/YTM32B1HA0.h"
    #include "YTM32B1HA0/include/YTM32B1HA0_features.h"
    #include "YTM32B1HA0/startup/system_YTM32B1HA0.h"
#elif defined(CPU_YTM32B1HA1)
    #define YTM32B1H_SERIES
    #include "YTM32B1HA1/include/YTM32B1HA1_features.h"
    #include "YTM32B1HA1/include/YTM32B1HA1.h"
    #include "YTM32B1HA1/startup/system_YTM32B1HA1.h"
#elif defined(CPU_YTM32B1HB0)
#define YTM32B1H_SERIES
#include "YTM32B1HB0/include/YTM32B1HB0_features.h"
#if defined(CPU_YTM32B1HB0_CSU)
#include "YTM32B1HB0/include/YTM32B1HB0_CSU.h"
#else
#include "YTM32B1HB0/include/YTM32B1HB0.h"
#endif
#include "YTM32B1HB0/startup/system_YTM32B1HB0.h"
#elif defined(CPU_YTM32Z1MC0)
    #define YTM32Z1M_SERIES
    #include "YTM32Z1MC0/include/YTM32Z1MC0.h"
    #include "YTM32Z1MC0/include/YTM32Z1MC0_features.h"
    #include "YTM32Z1MC0/startup/system_YTM32Z1MC0.h"
#elif defined(CPU_YTM32Z1MD0)
    #define YTM32Z1M_SERIES
    #include "YTM32Z1MD0/include/YTM32Z1MD0.h"
    #include "YTM32Z1MD0/include/YTM32Z1MD0_features.h"
    #include "YTM32Z1MD0/startup/system_YTM32Z1MD0.h"
#elif defined(CPU_YTM32Z1ME0)
#define YTM32Z1M_SERIES
#include "YTM32Z1ME0/include/YTM32Z1ME0.h"
#include "YTM32Z1ME0/include/YTM32Z1ME0_features.h"
#include "YTM32Z1ME0/startup/system_YTM32Z1ME0.h"
#elif defined(CPU_YTM32Z1DS0)
    #define YTM32Z1DS_SERIES
    #include "YTM32Z1DS0/include/YTM32Z1DS0.h"
    #include "YTM32Z1DS0/include/YTM32Z1DS0_features.h"
    #include "YTM32Z1DS0/startup/system_YTM32Z1DS0.h"
#elif defined(CPU_YTM32Z1LS0)
    #define YTM32B1L_SERIES
    #include "YTM32Z1LS0/include/YTM32Z1LS0.h"
    #include "YTM32Z1LS0/include/YTM32Z1LS0_features.h"
    #include "YTM32Z1LS0/startup/system_YTM32Z1LS0.h"
#elif defined(CPU_YTM32Z1LS1)
    #define YTM32B1L_SERIES
    #include "YTM32Z1LS1/include/YTM32Z1LS1.h"
    #include "YTM32Z1LS1/include/YTM32Z1LS1_features.h"
    #include "YTM32Z1LS1/startup/system_YTM32Z1LS1.h"
#else
    #error "No valid CPU defined!"
#endif

/*!
 * @brief Pull in the shared development-time assertion interface after SoC selection.
 */
#include "devassert.h"

/*! @} */ /* End of devices_device_registers */

#endif /* DEVICE_REGISTERS_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/

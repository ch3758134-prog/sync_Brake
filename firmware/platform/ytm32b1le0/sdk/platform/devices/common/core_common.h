/*
 * Copyright 2020-2025 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file core_common.h
 * @version 1.4.1
 * @brief Shared compiler and core utility macros for platform/devices.
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 0603 Rule 21.1: The macro identifier '%1s' is reserved.
 *
 * PRQA S 0602 Rule 1.3: The identifier '%1s' is reserved for use by the library.
 */

/*
 * Compiler identification macros used throughout platform/devices:
 *   __GNUC__        :  GNU Compiler Collection compatibility version
 *   __ghs__         :  Green Hills ARM Compiler
 *   __ICCARM__      :  IAR ARM Compiler
 *   __DCC__         :  Wind River Diab Compiler
 *   __ARMCC_VERSION :  ARM Compiler
 */

#ifndef CORE_COMMON_H
#define CORE_COMMON_H


#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @addtogroup devices_core_common
 * @brief Shared compiler, CPU, and section-placement helpers.
 * @details
 * This header centralizes low-level macros used by the shared startup code,
 * device headers, and peripheral drivers. It covers debug traps, interrupt
 * control, byte-swapping helpers, RAM-section annotations, and compiler
 * attributes shared across toolchains.
 * @{
 */

/*!
 * @name Debug and Core-Control Helpers
 * @brief Macros for breakpoints, interrupt control, and low-power entry.
 * @{
 */

/*!
 * @brief Trigger a breakpoint instruction on the active core.
 */
#define BKPT_ASM __asm("BKPT #0\n\t")

/*!
 * @brief Mark that the active toolchain targets a hardware floating-point unit.
 */
#if defined (__GNUC__) || defined (__ARMCC_VERSION)
#if defined (__VFP_FP__) && !defined (__SOFTFP__)
#define ENABLE_FPU
#endif

#elif defined (__ICCARM__)
#if defined __ARMVFP__
#define ENABLE_FPU
#endif

#elif defined (__ghs__) || defined (__DCC__)
#if defined (__VFP__)
#define ENABLE_FPU
#endif
#endif /* if defined (__GNUC__) */

/*!
 * @brief Enable IRQ handling globally.
 */
#if defined (__GNUC__)
#define ENABLE_INTERRUPTS() __asm volatile ("cpsie i" : : : "memory");
#else
#define ENABLE_INTERRUPTS() __asm("cpsie i")
#endif


/*!
 * @brief Disable IRQ handling globally.
 */
#if defined (__GNUC__)
#define DISABLE_INTERRUPTS() __asm volatile ("cpsid i" : : : "memory");
#else
#define DISABLE_INTERRUPTS() __asm("cpsid i")
#endif


/*!
 * @brief Enter standby until an interrupt wakes the core.
 */
#if defined (__GNUC__)
#define STANDBY() __asm volatile ("wfi")
#else
#define STANDBY() __asm("wfi")
#endif

/*!
 * @brief Execute a single no-operation instruction.
 */
#define NOP() __asm volatile ("nop")

/*! @} */ /* End of Debug and Core-Control Helpers */

/*!
 * @name Byte-Reordering Helpers
 * @brief Macros that reverse byte order for common word widths.
 * @{
 */

/*!
 * @brief Reverse the byte order of a 32-bit word.
 */
#if defined (__GNUC__) || defined (__ICCARM__) || defined (__ghs__) || defined (__ARMCC_VERSION)
#define REV_BYTES_32(a, b) __asm volatile ("rev %0, %1" : "=r" (b) : "r" (a))
#else
#define REV_BYTES_32(a, b) (b = ((a & 0xFF000000U) >> 24U) | ((a & 0xFF0000U) >> 8U) \
                                | ((a & 0xFF00U) << 8U) | ((a & 0xFFU) << 24U))
#endif

/*!
 * @brief Reverse the byte order inside each 16-bit halfword of a 32-bit value.
 */
#if defined (__GNUC__) || defined (__ICCARM__) || defined (__ghs__) || defined (__ARMCC_VERSION)
#define REV_BYTES_16(a, b) __asm volatile ("rev16 %0, %1" : "=r" (b) : "r" (a))
#else
#define REV_BYTES_16(a, b) (b = ((a & 0xFF000000U) >> 8U) | ((a & 0xFF0000U) << 8U) \
                                | ((a & 0xFF00U) >> 8U) | ((a & 0xFFU) << 8U))
#endif

/*! @} */ /* End of Byte-Reordering Helpers */

/*!
 * @name RAM Section Placement Helpers
 * @brief Macros used to declare and define functions that must execute from RAM.
 * @{
 */

/*!
 * @brief Annotate a function declaration so it is placed in the RAM code section.
 */
#if defined ( __GNUC__ ) || defined (__ARMCC_VERSION)
    #define START_FUNCTION_DECLARATION_RAMSECTION
    #define END_FUNCTION_DECLARATION_RAMSECTION        __attribute__((section (".code_ram")));
#elif defined ( __ghs__ )
    #define START_FUNCTION_DECLARATION_RAMSECTION      _Pragma("ghs callmode=far")
    #define END_FUNCTION_DECLARATION_RAMSECTION        __attribute__((section (".code_ram")));\
                                                       _Pragma("ghs callmode=default")
#elif defined ( __ICCARM__ )
    #define START_FUNCTION_DECLARATION_RAMSECTION
    #define END_FUNCTION_DECLARATION_RAMSECTION        __attribute__((section (".code_ram")));
#elif defined ( __DCC__ )
    #define START_FUNCTION_DECLARATION_RAMSECTION      _Pragma("section CODE \".code_ram\"") \
                                                       _Pragma("use_section CODE")
    #define END_FUNCTION_DECLARATION_RAMSECTION        ; \
                                                       _Pragma("section CODE \".text\"")
#else
    /* Keep compatibility with software analysis tools. */
    #define START_FUNCTION_DECLARATION_RAMSECTION
    #define END_FUNCTION_DECLARATION_RAMSECTION        ;
#endif

/* GCC, IAR, GHS, Diab, and Arm Compiler only need the section placement on
 * the declaration side, which keeps the definition compatible with analysis tools. */
    #define START_FUNCTION_DEFINITION_RAMSECTION
    #define END_FUNCTION_DEFINITION_RAMSECTION

/*!
 * @brief Disable IAR diagnostics that complain about RAM-section function calls.
 */
#if defined (__ICCARM__)
    #define DISABLE_CHECK_RAMSECTION_FUNCTION_CALL     _Pragma("diag_suppress=Ta022, Ta023")
    #define ENABLE_CHECK_RAMSECTION_FUNCTION_CALL      _Pragma("diag_default=Ta022, Ta023")
#else
    #define DISABLE_CHECK_RAMSECTION_FUNCTION_CALL
    #define ENABLE_CHECK_RAMSECTION_FUNCTION_CALL
#endif

/*! @} */ /* End of RAM Section Placement Helpers */

/*!
 * @name Core Identification and Attributes
 * @brief CPU-specific helpers and common compiler attributes.
 * @{
 */

/*!
 * @brief Return the active core identifier on supported multicore devices.
 */
#if defined(NUMBER_OF_CORES) && (NUMBER_OF_CORES > 1)
#if defined(CPU_YTM32B1HB0)
    /* Read the core identifier from the CIM->CORE_NUM register. */
    #define GET_CORE_ID()   (*((volatile uint32_t *)0x401ec074u) & 0x3U)
#endif /* CPU_YTM32B1HB0 */
#else
#define GET_CORE_ID()	0U
#endif /* NUMBER_OF_CORES */

/*!
 * @brief Request alignment for an object or type declaration.
 */
#if defined ( __GNUC__ ) || defined ( __ghs__ ) || defined ( __DCC__ ) || defined (__ARMCC_VERSION) || defined (__ICCARM__)
    #define ALIGNED(x)      __attribute__((aligned(x)))
#else
    /* Keep compatibility with software analysis tools. */
    #define ALIGNED(x)
#endif

/*!
 * @brief Expose common weak and packed attributes across toolchains.
 */
#if defined ( __GNUC__ ) || defined ( __ghs__ ) || defined ( __DCC__ ) || defined (__ARMCC_VERSION)
    #ifndef __weak
        #define __weak        __attribute__((weak)) /*PRQA S 0603, 0602*/
    #endif
    #ifndef __packed
        #define __packed      __attribute__((packed)) /*PRQA S 0603, 0602*/
    #endif
#else
    /* Keep compatibility with software analysis tools. */
    #ifndef __weak
        #define __weak
    #endif
    #ifndef __packed
        #define __packed
    #endif
#endif

/*!
 * @brief Mark the core as little-endian.
 */
#define CORE_LITTLE_ENDIAN

/*! @} */ /* End of Core Identification and Attributes */

/*! @} */ /* End of devices_core_common */

#ifdef __cplusplus
}
#endif

#endif /* CORE_COMMON_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/

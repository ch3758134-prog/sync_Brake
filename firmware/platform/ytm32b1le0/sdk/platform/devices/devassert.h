/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file devassert.h
 * @version 1.4.1
 * @brief Shared development-time assertion support for platform/devices.
 */

#ifndef DEVASSERT_H
#define DEVASSERT_H

#include <stdbool.h>

/*!
 * @addtogroup devices_devassert
 * @brief Development-time assertion helpers for shared device support code.
 * @details
 * This header provides the `DEV_ASSERT()` contract used throughout the shared
 * device layer to validate input parameters and check static invariants.
 * Because a failed validation usually indicates a software defect in the
 * application or integration code rather than a recoverable runtime error,
 * enabling these checks is recommended during development.
 *
 * The default implementation is controlled by the following compile-time
 * symbols:
 * - `DEV_ERROR_DETECT`
 *   - Defined: validations are enabled. A failed condition triggers
 *     `BKPT_ASM` and then halts execution in an infinite loop so the fault
 *     can be inspected in a debugger.
 *   - Undefined: `DEV_ASSERT()` becomes `((void)0)`, removing the runtime
 *     checking overhead from optimized production builds.
 * - `CUSTOM_DEVASSERT`
 *   - Define this symbol as a quoted header name such as
 *     `"custom_devassert.h"` to provide an application-specific backend.
 *     This allows the project to replace the default breakpoint behavior with
 *     logging, reset, safe-state entry, or another policy.
 * @{
 */

/*!
 * @brief Use an application-provided `DEV_ASSERT()` implementation.
 *
 * Define `CUSTOM_DEVASSERT` as a quoted header name when the application wants
 * to provide its own assertion backend. When this symbol is defined, the
 * custom header is included directly and the default breakpoint-based
 * implementation is bypassed entirely.
 */
#if defined (CUSTOM_DEVASSERT)
    /* Include the application-provided assertion backend. */
    #include CUSTOM_DEVASSERT
#elif defined (DEV_ERROR_DETECT)
    /*!
     * @brief Trap execution when a development assertion fails.
     *
     * @param[in] x Condition to validate.
     *
     * When @a x is false, the function triggers `BKPT_ASM` and then loops
     * forever so the failure can be inspected in a debugger.
     */
static inline void DevAssert(volatile bool x)
{
    if(x) { } else { BKPT_ASM; for(;;) {} }
}

    /*!
     * @brief Validate a development-time assumption.
     */
    #define DEV_ASSERT(x) DevAssert(x)
#else
    /*!
     * @brief Compile `DEV_ASSERT()` to a no-op when runtime checking is disabled.
     */
    #define DEV_ASSERT(x) ((void)0)
#endif

/*! @} */ /* End of devices_devassert */

#endif /* DEVASSERT_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/

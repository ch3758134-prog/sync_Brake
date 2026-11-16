/*
 * Copyright (c) 2021-2025 Yuntu Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*!
 * @file floats.h
 * @version 1.4.1
 * @brief Fixed-width floating-point aliases for platform/devices.
 */

#ifndef FLOATS_H
#define FLOATS_H

#ifdef __cplusplus
extern "C"{
#endif

/*!
 * @addtogroup devices_floats
 * @brief Floating-point typedefs shared by device headers and drivers.
 * @details
 * This header provides concise aliases for the floating-point widths commonly
 * referenced by shared platform code and peripheral drivers.
 * @{
 */

/*!
 * @brief 32-bit single-precision floating-point alias.
 */
typedef float float32_t;

/*!
 * @brief 64-bit double-precision floating-point alias.
 */
typedef double float64_t;

/*! @} */ /* End of devices_floats */

#ifdef __cplusplus
}
#endif

#endif /* FLOATS_H */

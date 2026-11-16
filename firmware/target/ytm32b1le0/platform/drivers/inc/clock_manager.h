/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file clock_manager.h
 * @version 1.5.0
 *
 * @brief Clock Manager - Top-level include for clock configuration API.
 *
 * This header is the single public entry point for the Clock Manager module.
 * It includes the device-dispatching header (clock.h), which in turn pulls
 * in the correct device-specific clock implementation based on the CPU macro.
 *
 * Application code should include this header and use the CLOCK_DRV_* and
 * CLOCK_SYS_* APIs declared in clock.h and the device header.
 */

#ifndef CLOCK_MANAGER_H
#define CLOCK_MANAGER_H

#include "clock.h"

#endif /* CLOCK_MANAGER_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

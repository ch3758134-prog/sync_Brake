/*
 * Copyright 2020-2026 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file clock_config.c
 * @brief 
 * 
 */

#include "stddef.h"
#include "clock_config.h"

/*! @brief peripheral clock PeripheralClockConfig */

peripheral_clock_config_t clock_config0PeripheralClockConfig[2] = {
    {
        .clkName = GPIO_CLK,
        .clkGate = true,
        .divider = DIV_BY_1,
        .clkSrc = CLK_SRC_FIRC,
    },
    {
        .clkName = UART1_CLK,
        .clkGate = true,
        .divider = DIV_BY_1,
        .clkSrc = CLK_SRC_FXOSC,
    },
};

const scu_config_t clock_config0ScuConfig = {
    .fircEnable = true,
    .fircDeepSleepEnable = false,
    .sircDeepSleepEnable = false,
    .sircStandbyEnable = false,
    .sysClkSrc = SCU_SYSTEM_CLOCK_SRC_FIRC,
    .fxoscConfig =
        {
            .enable = true,
            .bypassMode = false,
            .gainSelection = 6,
            .frequency = 24000000U,
        },
    .sxoscConfig = 
        {
            .enable = false,
            .gainSelection = 0,
            .bypassMode = false,
            .deepsleepEnable = false,
            .standbyEnable = false,
            .frequency = 32768U,
        },
    .sysDiv = SCU_SYS_CLK_DIV_BY_1,
    .fastBusDiv = SCU_SYS_CLK_DIV_BY_1,
    .slowBusDiv = SCU_SYS_CLK_DIV_BY_2,
    .flashDiv = SCU_SYS_CLK_DIV_BY_2,
    .clockOutConfig =
        {
            .enable = false,
            .source = SCU_CLKOUT_SEL_FIRC_CLK,
            .divider = 1
        },
};

const cmu_config_t clock_config0CmuConfig = {
    .fircClockMonitor={
        .enable = true,                 
        .resetEnable = true,           
        .compareHigh = (60 * 128 / 2),        
        .compareLow = (36 * 128 / 2),  
    },
    .fxoscClockMonitor={
        .enable = true,                 
        .resetEnable = true,             
        .compareHigh = (30 * 128 / 2),        
        .compareLow = (18 * 128 / 2),         
    },
};

/*! @brief User Configuration structure clock_config0ClockManager */
clock_manager_user_config_t clock_config0ClockManager = {
    .scuConfigPtr = &clock_config0ScuConfig,
    .cmuConfigPtr = &clock_config0CmuConfig,
    .ipcConfig =
        {
            .peripheralClocks = clock_config0PeripheralClockConfig,
            .count = 2,
        },
};

/*! @brief Array of pointers to User configuration structures */
clock_manager_user_config_t const *g_clockManConfigsArr[] = {
    &clock_config0ClockManager,
};

/*! @brief Array of pointers to User defined Callbacks configuration structures */
/* The tool do not support generate Callbacks configuration. It's always empty. */
clock_manager_callback_user_config_t *g_clockManCallbacksArr[] = {(void *)0};

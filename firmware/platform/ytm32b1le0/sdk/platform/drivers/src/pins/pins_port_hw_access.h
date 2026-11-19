/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file pins_port_hw_access.h
 * @version 1.5.0
 *
 * @brief PINS PORT Hardware Access Layer.
 *
 * This internal header provides low-level helpers for programming the PCTRL
 * registers used by the PINS module. The APIs in this file are used by the
 * public driver layer and should not be called directly by application code.
 */

#ifndef PINS_PCTRL_HW_ACCESS_H
#define PINS_PCTRL_HW_ACCESS_H

#include "pins_driver.h"

/*!
 * @addtogroup pins_port_hw_access PINS PORT Hardware Access
 * @ingroup pins
 * @brief Low-level PCTRL register access helpers for the PINS module.
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * Pin Initialization
 ******************************************************************************/
/*!
 * @name Pin Initialization
 * @brief Functions for applying complete pin-configuration records.
 * @{
 */

/*!
 * @brief Apply one pin configuration record to the hardware registers.
 *
 * Programs the selected PCR register, optional digital-filter settings, GPIO
 * direction, interrupt mode, and optional register lock state.
 *
 * @param[in] config  Pointer to the pin configuration record.
 */
void PINS_Init(const pin_settings_config_t * config);

/*! @} */ /* End of Pin Initialization */

/*******************************************************************************
 * Port Control
 ******************************************************************************/
/*!
 * @name Port Control
 * @brief Functions for updating specific PCTRL PCR fields.
 * @{
 */

#if FEATURE_PINS_HAS_PULL_SELECTION
/*!
 * @brief Configure the internal pull resistor for a pin.
 *
 * @param[in] base        Port control base pointer.
 * @param[in] pin         Zero-based pin index inside the port.
 * @param[in] pullConfig  Pull resistor configuration to apply.
 */
static inline void PINS_SetPullSel(PCTRL_Type * const base,
                                   uint32_t pin,
                                   port_pull_config_t pullConfig)
{
    DEV_ASSERT(pin < PCTRL_PCR_COUNT);

    switch (pullConfig)
    {
        case PCTRL_INTERNAL_PULL_NOT_ENABLED:
            {
                base->PCR[pin] &= ~(PCTRL_PCR_PE_MASK);
            }
            break;
        case PCTRL_INTERNAL_PULL_DOWN_ENABLED:
            {
                uint32_t regValue = base->PCR[pin];
                regValue &= ~(PCTRL_PCR_PS_MASK);
                regValue |= PCTRL_PCR_PE(1U);
                base->PCR[pin] = regValue;
            }
            break;
        case PCTRL_INTERNAL_PULL_UP_ENABLED:
            {
                uint32_t regValue = base->PCR[pin];
                regValue |= PCTRL_PCR_PE(1U);
                regValue |= PCTRL_PCR_PS(1U);
                base->PCR[pin] = regValue;
            }
            break;
        default:
            /* invalid command */
            DEV_ASSERT(false);
            break;
    }
}
#endif /* FEATURE_PINS_HAS_PULL_SELECTION */

/*!
 * @brief Select the mux mode for a pin.
 *
 * Updates only the mux field of the selected PCR register.
 *
 * @param[in] base  Port control base pointer.
 * @param[in] pin   Zero-based pin index inside the port.
 * @param[in] mux   Mux selection to apply.
 */
void PINS_SetMuxModeSel(PCTRL_Type * const base,
                        uint32_t pin,
                        port_mux_t mux);

/*! @} */ /* End of Port Control */

#if defined(__cplusplus)
}
#endif

/*! @} */ /* End of pins_port_hw_access group */

#endif /* PINS_PCTRL_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

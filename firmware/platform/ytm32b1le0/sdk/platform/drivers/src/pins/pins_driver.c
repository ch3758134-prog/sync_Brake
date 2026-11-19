/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file pins_driver.c
 * @version 1.5.0
 *
 * @brief PINS Driver - implementation of the public PINS_DRV_* API.
 *
 * This file implements the driver-layer wrappers declared in pins_driver.h.
 * Each function forwards the request to the corresponding GPIO or PCTRL
 * hardware access helper without changing the application-facing API.
 */

#include "device_registers.h"
#include "pins_gpio_hw_access.h"
#include "pins_port_hw_access.h"

/*******************************************************************************
 * Initialization
 ******************************************************************************/

/*!
 * @brief Initialize a list of pins from an array of configuration records.
 */
status_t PINS_DRV_Init(uint32_t pinCount,
                       const pin_settings_config_t config[])
{
    uint32_t i;

    for (i = 0U; i < pinCount; i++)
    {
        PINS_Init(&config[i]);
    }

    return STATUS_SUCCESS;
}

/*******************************************************************************
 * Port Control
 ******************************************************************************/

#if FEATURE_PINS_HAS_PULL_SELECTION
/*!
 * @brief Configure the internal pull resistor for a pin.
 */
void PINS_DRV_SetPullSel(PCTRL_Type * const base,
                         uint32_t pin,
                         port_pull_config_t pullConfig)
{
    PINS_SetPullSel(base, pin, pullConfig);
}
#endif /* FEATURE_PINS_HAS_PULL_SELECTION */

/*!
 * @brief Select the mux mode for a pin.
 */
void PINS_DRV_SetMuxModeSel(PCTRL_Type * const base,
                            uint32_t pin,
                            port_mux_t mux)
{
    PINS_SetMuxModeSel(base, pin, mux);
}

/*******************************************************************************
 * Interrupt & Digital Filter Control
 ******************************************************************************/

/*!
 * @brief Configure the interrupt or DMA trigger mode of a pin.
 */
void PINS_DRV_SetPinIntSel(GPIO_Type * const base,
                           uint32_t pin,
                           gpio_interrupt_config_t intConfig)
{
    PINS_SetPinIntSel(base, pin, intConfig);
}

/*!
 * @brief Read the current interrupt or DMA trigger configuration of a pin.
 */
gpio_interrupt_config_t PINS_DRV_GetPinIntSel(const GPIO_Type * const base,
                                              uint32_t pin)
{
    return PINS_GetPinIntSel(base, pin);
}

/*!
 * @brief Clear the interrupt status flag of one pin.
 */
void PINS_DRV_ClearPinIntFlagCmd(GPIO_Type * const base,
                                 uint32_t pin)
{
    PINS_ClearPinIntFlagCmd(base, pin);
}

#if defined(FEATURE_PINS_HAS_DIGITAL_FILTER) && (FEATURE_PINS_HAS_DIGITAL_FILTER)
/*!
 * @brief Enable the digital filter for one pin.
 */
void PINS_DRV_EnableDigitalFilter(GPIO_Type * const base,
                                  uint32_t pin)
{
    PINS_EnableDigitalFilter(base, pin);
}

/*!
 * @brief Disable the digital filter for one pin.
 */
void PINS_DRV_DisableDigitalFilter(GPIO_Type * const base,
                                   uint32_t pin)
{
    PINS_DisableDigitalFilter(base, pin);
}

/*!
 * @brief Program the digital filter settings for one pin.
 */
void PINS_DRV_ConfigDigitalFilter(GPIO_Type * const base,
                                  const gpio_digital_filter_config_t * const config,
                                  uint32_t pin)
{
    PINS_ConfigDigitalFilter(base, config, pin);
}
#endif /* FEATURE_PINS_HAS_DIGITAL_FILTER */

/*!
 * @brief Read the interrupt status flags for an entire GPIO port.
 */
uint32_t PINS_DRV_GetPortIntFlag(const GPIO_Type * const base)
{
    return PINS_GetPortIntFlag(base);
}

/*!
 * @brief Clear all interrupt status flags for a GPIO port.
 */
void PINS_DRV_ClearPortIntFlagCmd(GPIO_Type * const base)
{
    PINS_ClearPortIntFlagCmd(base);
}

/*******************************************************************************
 * GPIO Direction Control
 ******************************************************************************/

/*!
 * @brief Read the current direction mask of a GPIO port.
 */
pins_channel_type_t PINS_DRV_GetPinsDirection(const GPIO_Type * const base)
{
    return PINS_GPIO_GetPinsDirection(base);
}

/*!
 * @brief Set the direction of one GPIO pin.
 */
void PINS_DRV_SetPinDirection(GPIO_Type * const base,
                              pins_channel_type_t pin,
                              pin_direction_t direction)
{
    PINS_GPIO_SetPinDirection(base, pin, direction);
}

/*!
 * @brief Set the direction mask of an entire GPIO port.
 */
void PINS_DRV_SetPinsDirection(GPIO_Type * const base,
                               pins_channel_type_t pins)
{
    PINS_GPIO_SetPinsDirection(base, pins);
}

#if FEATURE_PINS_HAS_INPUT_DISABLE
/*!
 * @brief Enable or disable the input path for selected pins.
 */
void PINS_DRV_SetPortInputDisable(GPIO_Type * const base,
                                  pins_channel_type_t pins)
{
    PINS_GPIO_SetPortInputDisable(base, pins);
}

/*!
 * @brief Read the input-disable mask of a GPIO port.
 */
pins_channel_type_t PINS_DRV_GetPortInputDisable(const GPIO_Type * const base)
{
    return PINS_GPIO_GetPortInputDisable(base);
}
#endif /* FEATURE_PINS_HAS_INPUT_DISABLE */

/*******************************************************************************
 * GPIO Data Access
 ******************************************************************************/

/*!
 * @brief Write one GPIO pin with a logic level.
 */
void PINS_DRV_WritePin(GPIO_Type * const base,
                       pins_channel_type_t pin,
                       pins_level_type_t value)
{
    PINS_GPIO_WritePin(base, pin, value);
}

/*!
 * @brief Write the output register of an entire GPIO port.
 */
void PINS_DRV_WritePins(GPIO_Type * const base,
                        pins_channel_type_t pins)
{
    PINS_GPIO_WritePins(base, pins);
}

/*!
 * @brief Read the current output register value of a GPIO port.
 */
pins_channel_type_t PINS_DRV_GetPinsOutput(const GPIO_Type * const base)
{
    return PINS_GPIO_GetPinsOutput(base);
}

/*!
 * @brief Set selected GPIO output pins to logic high.
 */
void PINS_DRV_SetPins(GPIO_Type * const base,
                      pins_channel_type_t pins)
{
    PINS_GPIO_SetPins(base, pins);
}

/*!
 * @brief Clear selected GPIO output pins to logic low.
 */
void PINS_DRV_ClearPins(GPIO_Type * const base,
                        pins_channel_type_t pins)
{
    PINS_GPIO_ClearPins(base, pins);
}

/*!
 * @brief Toggle selected GPIO output pins.
 */
void PINS_DRV_TogglePins(GPIO_Type * const base,
                         pins_channel_type_t pins)
{
    PINS_GPIO_TogglePins(base, pins);
}

/*!
 * @brief Read the input value of an entire GPIO port.
 */
pins_channel_type_t PINS_DRV_ReadPins(const GPIO_Type * const base)
{
    return PINS_GPIO_ReadPins(base);
}

/*!
 * @brief Read the input value of one GPIO pin.
 */
pins_level_type_t PINS_DRV_ReadPin(const GPIO_Type * const base,
                                   pins_channel_type_t pin)
{
    uint32_t portValue = 0x0U;
    pins_level_type_t pinValue = 0x0U;

    portValue = PINS_GPIO_ReadPins(base);
    pinValue = (pins_level_type_t)((portValue >> pin) & 0x01U);

    return pinValue;
}

/******************************************************************************
 * EOF
 *****************************************************************************/

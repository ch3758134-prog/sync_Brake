/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file pins_gpio_hw_access.h
 * @version 1.5.0
 *
 * @brief PINS GPIO Hardware Access Layer.
 *
 * This internal header provides low-level inline accessors for GPIO direction,
 * GPIO data, pin interrupt configuration, and optional digital filter control.
 * The public PINS driver wraps these helpers and exposes the application-level
 * `PINS_DRV_*` API.
 */

#ifndef PINS_GPIO_HW_ACCESS_H
#define PINS_GPIO_HW_ACCESS_H

#include "pins_driver.h"

/*!
 * @addtogroup pins_gpio_hw_access PINS GPIO Hardware Access
 * @ingroup pins
 * @brief Low-level GPIO register access helpers for the PINS module.
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * GPIO Direction Control
 ******************************************************************************/
/*!
 * @name GPIO Direction Control
 * @brief Functions for reading and programming GPIO direction state.
 * @{
 */

/*!
 * @brief Read the current direction mask of a GPIO port.
 *
 * Each bit in the returned value corresponds to one pin:
 *   - `0`: pin is configured as input
 *   - `1`: pin is configured as output
 *
 * @param[in] base  GPIO base pointer.
 * @return Current direction mask.
 */
static inline pins_channel_type_t PINS_GPIO_GetPinsDirection(const GPIO_Type * const base)
{
    return (pins_channel_type_t)base->POER;
}

/*!
 * @brief Set the direction of one GPIO pin.
 *
 * @param[in] base       GPIO base pointer.
 * @param[in] pin        Zero-based pin index inside the port.
 * @param[in] direction  Direction value to apply to the selected pin.
 */
static inline void PINS_GPIO_SetPinDirection(GPIO_Type * const base,
                                             pins_channel_type_t pin,
                                             pin_direction_t direction)
{
    pins_channel_type_t pinsDirections = (pins_channel_type_t)base->POER;

    pinsDirections &= (pins_channel_type_t)(~((pins_channel_type_t)1U << pin));
    pinsDirections |= (pins_channel_type_t)((pins_channel_type_t)direction << pin);
    base->POER = GPIO_POER_POE(pinsDirections);
}

/*!
 * @brief Set the direction mask of an entire GPIO port.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pins  Direction mask to write.
 */
static inline void PINS_GPIO_SetPinsDirection(GPIO_Type * const base,
                                              pins_channel_type_t pins)
{
    base->POER = GPIO_POER_POE(pins);
}

#if FEATURE_PINS_HAS_INPUT_DISABLE
/*!
 * @brief Enable or disable the input path for selected pins.
 *
 * Each bit in `pins` corresponds to one pin:
 *   - `0`: input path remains enabled
 *   - `1`: input path is disabled
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pins  Input-disable mask.
 */
static inline void PINS_GPIO_SetPortInputDisable(GPIO_Type * const base,
                                                 pins_channel_type_t pins)
{
    base->PIER = GPIO_PIER_PIE(~pins);
}

/*!
 * @brief Read the input-disable mask of a GPIO port.
 *
 * @param[in] base  GPIO base pointer.
 * @return Current input-disable mask.
 */
static inline pins_channel_type_t PINS_GPIO_GetPortInputDisable(const GPIO_Type * const base)
{
    return ~(pins_channel_type_t)base->PIER;
}
#endif /* FEATURE_PINS_HAS_INPUT_DISABLE */

/*! @} */ /* End of GPIO Direction Control */

/*******************************************************************************
 * GPIO Data Access
 ******************************************************************************/
/*!
 * @name GPIO Data Access
 * @brief Functions for reading and driving GPIO data values.
 * @{
 */

/*!
 * @brief Write the output register of an entire GPIO port.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pins  Output value mask to write.
 */
static inline void PINS_GPIO_WritePins(GPIO_Type * const base,
                                       pins_channel_type_t pins)
{
    base->PDOR = GPIO_PDOR_PDO(pins);
}

/*!
 * @brief Read the current output register value of a GPIO port.
 *
 * @param[in] base  GPIO base pointer.
 * @return Current output latch value.
 */
static inline pins_channel_type_t PINS_GPIO_GetPinsOutput(const GPIO_Type * const base)
{
    pins_channel_type_t returnValue = 0U;

    returnValue = (pins_channel_type_t)(base->PDOR);

    return returnValue;
}

/*!
 * @brief Set selected GPIO output pins to logic high.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pins  Mask of pins to set.
 */
static inline void PINS_GPIO_SetPins(GPIO_Type * const base,
                                     pins_channel_type_t pins)
{
    base->PSOR = GPIO_PSOR_PSO(pins);
}

/*!
 * @brief Clear selected GPIO output pins to logic low.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pins  Mask of pins to clear.
 */
static inline void PINS_GPIO_ClearPins(GPIO_Type * const base,
                                       pins_channel_type_t pins)
{
    base->PCOR = GPIO_PCOR_PCO(pins);
}

/*!
 * @brief Write one GPIO pin with a logic level.
 *
 * @param[in] base   GPIO base pointer.
 * @param[in] pin    Zero-based pin index inside the port.
 * @param[in] value  Logic level to drive on the selected pin.
 */
static inline void PINS_GPIO_WritePin(GPIO_Type * const base,
                                      pins_channel_type_t pin,
                                      pins_level_type_t value)
{
    if (value == ((pins_level_type_t)0x00))
    {
        PINS_GPIO_ClearPins(base, (0x1UL) << pin);
    }
    else if (value == ((pins_level_type_t)0x01))
    {
        PINS_GPIO_SetPins(base, (0x1UL) << pin);
    }
    else
    {
        /* NONE */
    }
}

/*!
 * @brief Toggle selected GPIO output pins.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pins  Mask of pins to toggle.
 */
static inline void PINS_GPIO_TogglePins(GPIO_Type * const base,
                                        pins_channel_type_t pins)
{
    base->PTOR = GPIO_PTOR_PTO(pins);
}

/*!
 * @brief Read the input value of an entire GPIO port.
 *
 * @param[in] base  GPIO base pointer.
 * @return Current sampled input levels.
 */
static inline pins_channel_type_t PINS_GPIO_ReadPins(const GPIO_Type * const base)
{
    pins_channel_type_t returnValue = 0U;

    returnValue = (pins_channel_type_t)(base->PDIR);

    return returnValue;
}

/*! @} */ /* End of GPIO Data Access */

/*******************************************************************************
 * Interrupt & Digital Filter Control
 ******************************************************************************/
/*!
 * @name Interrupt & Digital Filter Control
 * @brief Functions for pin interrupt routing, interrupt flags, and digital
 *        filter programming.
 * @{
 */

/*!
 * @brief Configure the interrupt or DMA trigger mode of a pin.
 *
 * @param[in] base       GPIO base pointer.
 * @param[in] pin        Zero-based pin index inside the port.
 * @param[in] intConfig  Interrupt or DMA trigger configuration.
 */
static inline void PINS_SetPinIntSel(GPIO_Type * const base,
                                     uint32_t pin,
                                     gpio_interrupt_config_t intConfig)
{
    DEV_ASSERT(pin < PCTRL_PCR_COUNT);

    uint32_t regValue = base->PCR[pin];

    regValue &= ~(GPIO_PCR_IRQC_MASK);
    regValue |= GPIO_PCR_IRQC(intConfig);
    base->PCR[pin] = regValue;
}

/*!
 * @brief Read the current interrupt or DMA trigger configuration of a pin.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pin   Zero-based pin index inside the port.
 * @return Current interrupt or DMA trigger configuration.
 */
static inline gpio_interrupt_config_t PINS_GetPinIntSel(const GPIO_Type * const base,
                                                        uint32_t pin)
{
    DEV_ASSERT(pin < PCTRL_PCR_COUNT);

    uint32_t regValue = base->PCR[pin];

    regValue = (regValue & GPIO_PCR_IRQC_MASK) >> GPIO_PCR_IRQC_SHIFT;

    return (gpio_interrupt_config_t)regValue;
}

/*!
 * @brief Clear the interrupt status flag of one pin.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pin   Zero-based pin index inside the port.
 */
static inline void PINS_ClearPinIntFlagCmd(GPIO_Type * const base,
                                           uint32_t pin)
{
    DEV_ASSERT(pin < PCTRL_PCR_COUNT);
    base->PIFR = ((uint32_t)1) << pin;
}

#if FEATURE_PINS_HAS_DIGITAL_FILTER
/*!
 * @brief Enable the GPIO-owned digital filter for one pin.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pin   Zero-based pin index inside the port.
 */
static inline void PINS_EnableDigitalFilter(GPIO_Type * const base,
                                            uint32_t pin)
{
    DEV_ASSERT(pin < PCTRL_PCR_COUNT);
    base->PCR[pin] |= GPIO_PCR_DFE_MASK;
}

/*!
 * @brief Disable the GPIO-owned digital filter for one pin.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pin   Zero-based pin index inside the port.
 */
static inline void PINS_DisableDigitalFilter(GPIO_Type * const base,
                                             uint32_t pin)
{
    DEV_ASSERT(pin < PCTRL_PCR_COUNT);
    base->PCR[pin] &= ~GPIO_PCR_DFE_MASK;
}

/*!
 * @brief Program the GPIO-owned digital filter settings for one pin.
 *
 * @param[in] base    GPIO base pointer.
 * @param[in] config  Pointer to the digital filter configuration structure.
 * @param[in] pin     Zero-based pin index inside the port.
 */
static inline void PINS_ConfigDigitalFilter(GPIO_Type * const base,
                                            const gpio_digital_filter_config_t * const config,
                                            uint32_t pin)
{
#if defined(FEATURE_PINS_HAS_FILTER_CLOCK_SELECTION) && (FEATURE_PINS_HAS_FILTER_CLOCK_SELECTION == 1)
    base->PCR[pin] &= ~GPIO_PCR_DFCS_MASK;
    base->PCR[pin] |= GPIO_PCR_DFCS(config->clock);
#endif
    base->PCR[pin] &= ~GPIO_PCR_DFW_MASK;
    base->PCR[pin] |= GPIO_PCR_DFW(config->width);
}
#endif /* FEATURE_PINS_HAS_DIGITAL_FILTER */

#if defined(FEATURE_PCTRL_HAS_DIGITAL_FILTER) && (FEATURE_PCTRL_HAS_DIGITAL_FILTER == 1)
/*!
 * @brief Enable the PCTRL-owned digital filter for one pin.
 *
 * @param[in] base  Port control base pointer.
 * @param[in] pin   Zero-based pin index inside the port.
 */
static inline void PINS_EnableDigitalFilter(PCTRL_Type * const base,
                                            uint32_t pin)
{
    DEV_ASSERT(pin < PCTRL_PCR_COUNT);
    base->PCR[pin] |= PCTRL_PCR_DFE_MASK;
}

/*!
 * @brief Disable the PCTRL-owned digital filter for one pin.
 *
 * @param[in] base  Port control base pointer.
 * @param[in] pin   Zero-based pin index inside the port.
 */
static inline void PINS_DisableDigitalFilter(PCTRL_Type * const base,
                                             uint32_t pin)
{
    DEV_ASSERT(pin < PCTRL_PCR_COUNT);
    base->PCR[pin] &= ~PCTRL_PCR_DFE_MASK;
}

/*!
 * @brief Program the PCTRL-owned digital filter settings for one pin.
 *
 * @param[in] base    Port control base pointer.
 * @param[in] config  Pointer to the digital filter configuration structure.
 * @param[in] pin     Zero-based pin index inside the port.
 */
static inline void PINS_ConfigDigitalFilter(PCTRL_Type * const base,
                                            const gpio_digital_filter_config_t * const config,
                                            uint32_t pin)
{
    base->PCR[pin] &= ~PCTRL_PCR_DFW_MASK;
    base->PCR[pin] |= PCTRL_PCR_DFW(config->width);
}
#endif /* FEATURE_PCTRL_HAS_DIGITAL_FILTER */

/*!
 * @brief Read the interrupt status flags for an entire GPIO port.
 *
 * @param[in] base  GPIO base pointer.
 * @return Bit mask of pending interrupt flags for the port.
 */
static inline uint32_t PINS_GetPortIntFlag(const GPIO_Type * const base)
{
    uint32_t regValue = base->PIFR;

    return regValue;
}

/*!
 * @brief Clear all interrupt status flags for a GPIO port.
 *
 * @param[in] base  GPIO base pointer.
 */
static inline void PINS_ClearPortIntFlagCmd(GPIO_Type * const base)
{
    base->PIFR = GPIO_PIFR_PIF_MASK;
}

/*! @} */ /* End of Interrupt & Digital Filter Control */

#if defined(__cplusplus)
}
#endif

/*! @} */ /* End of pins_gpio_hw_access group */

#endif /* PINS_GPIO_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

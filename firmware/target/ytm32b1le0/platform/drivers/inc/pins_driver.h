/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file pins_driver.h
 * @version 1.5.0
 *
 * @brief PINS Driver - public API for pin mux, GPIO, and pin interrupt control.
 *
 * This header defines the application-level interface for configuring pad mux,
 * electrical attributes, GPIO direction and data, and per-pin interrupt or
 * DMA trigger settings. The driver wraps the lower-level PCTRL and GPIO
 * hardware access helpers found in the internal pins source directory.
 *
 * The APIs are organized into five categories:
 *   - **Initialization**: Apply one or more pin configuration records.
 *   - **Port Control**: Update mux selection and pin electrical settings.
 *   - **Interrupt & Digital Filter Control**: Manage per-pin interrupt
 *     routing, status flags, and optional digital filters.
 *   - **GPIO Direction Control**: Configure input/output direction and input
 *     disable state when supported by the device.
 *   - **GPIO Data Access**: Read, write, set, clear, and toggle GPIO pins.
 */

#ifndef PINS_DRIVER_H
#define PINS_DRIVER_H

#include <stddef.h>
#include "device_registers.h"
#include "status.h"

/*!
 * @addtogroup pins
 * @brief Pin control and GPIO driver - public API.
 * @details Provides the configuration structures and helper functions used to
 *          initialize pads, select alternate functions, configure GPIO
 *          direction, and manage pin interrupt behavior.
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief Bit-mask type used for GPIO port channels.
 *
 * Each bit in this type corresponds to one pin in a GPIO port:
 *   - bit value `0` clears or reports a low/input-disabled state
 *   - bit value `1` sets or reports a high/enabled state
 */
typedef uint32_t pins_channel_type_t;

/*!
 * @brief Logic level type used for single-pin read and write operations.
 *
 * The driver interprets `0U` as logic low and `1U` as logic high.
 */
typedef uint8_t pins_level_type_t;

/*!
 * @brief GPIO direction selection.
 *
 * | Value                        | Description                                           |
 * |-----------------------------|-------------------------------------------------------|
 * | GPIO_INPUT_DIRECTION        | Configure the pin as a GPIO input.                    |
 * | GPIO_OUTPUT_DIRECTION       | Configure the pin as a GPIO output.                   |
 * | GPIO_UNSPECIFIED_DIRECTION  | Placeholder used when GPIO direction is not updated.  |
 *
 * @note `GPIO_UNSPECIFIED_DIRECTION` is only meaningful in configuration
 *       records for non-GPIO mux selections. Do not pass it to runtime GPIO
 *       direction APIs.
 */
typedef enum
{
    GPIO_INPUT_DIRECTION = 0x0U,          /*!< Configure the pin as a GPIO input. */
    GPIO_OUTPUT_DIRECTION = 0x1U,         /*!< Configure the pin as a GPIO output. */
    GPIO_UNSPECIFIED_DIRECTION = 0x2U     /*!< Leave GPIO direction unspecified. */
} pin_direction_t;

#if FEATURE_PINS_HAS_PULL_SELECTION
/*!
 * @brief Internal pull resistor selection.
 *
 * Selects whether the internal pull network for a digital pin is disabled,
 * driven low, or driven high.
 */
typedef enum
{
    PCTRL_INTERNAL_PULL_NOT_ENABLED = 0U,     /*!< Disable the internal pull resistor. */
    PCTRL_INTERNAL_PULL_DOWN_ENABLED = 1U,    /*!< Enable the internal pull-down resistor. */
    PCTRL_INTERNAL_PULL_UP_ENABLED = 2U       /*!< Enable the internal pull-up resistor. */
} port_pull_config_t;
#endif /* FEATURE_PINS_HAS_PULL_SELECTION */

#if FEATURE_PINS_HAS_OPEN_DRAIN
/*!
 * @brief Output stage selection for pins that support open-drain mode.
 */
typedef enum
{
    PCTRL_OPEN_DRAIN_DISABLED = 0U,       /*!< Use the normal CMOS output driver. */
    PCTRL_OPEN_DRAIN_ENABLED = 1U         /*!< Use open-drain output behavior. */
} port_open_drain_t;
#endif /* FEATURE_PINS_HAS_OPEN_DRAIN */

#if FEATURE_PINS_HAS_FILTER_CLOCK_SEL
/*!
 * @brief Digital filter clock source selector.
 */
typedef enum
{
    PIN_FILETER_USE_BUS_CLOCK = 0U,       /*!< Clock the filter from the GPIO bus clock. */
    PIN_FILTER_USE_FUNCTION_CLOCK = 1U    /*!< Clock the filter from the GPIO function clock. */
} pin_filter_clock_sel_t;
#endif /* FEATURE_PINS_HAS_FILTER_CLOCK_SEL */

#if FEATURE_PINS_HAS_DRIVE_STRENGTH
/*!
 * @brief Output drive strength selection.
 */
typedef enum
{
    PCTRL_LOW_DRIVE_STRENGTH = 0U,        /*!< Select low drive strength. */
    PCTRL_HIGH_DRIVE_STRENGTH = 1U        /*!< Select high drive strength. */
} port_drive_strength_t;
#endif /* FEATURE_PINS_HAS_DRIVE_STRENGTH */

/*!
 * @brief Pin mux selection.
 *
 * Selects whether a pad is routed to analog mode, GPIO mode, or one of the
 * chip-specific alternate functions exposed by the device.
 */
typedef enum
{
    PCTRL_PIN_DISABLED = 0U,      /*!< Disable the digital path and use the pin as analog. */
    PCTRL_MUX_AS_GPIO = 1U,       /*!< Route the pin to the GPIO peripheral. */
    PCTRL_MUX_ALT2 = 2U,          /*!< Route the pin to alternate function 2. */
    PCTRL_MUX_ALT3 = 3U,          /*!< Route the pin to alternate function 3. */
    PCTRL_MUX_ALT4 = 4U,          /*!< Route the pin to alternate function 4. */
    PCTRL_MUX_ALT5 = 5U,          /*!< Route the pin to alternate function 5. */
    PCTRL_MUX_ALT6 = 6U,          /*!< Route the pin to alternate function 6. */
    PCTRL_MUX_ALT7 = 7U,          /*!< Route the pin to alternate function 7. */
#if defined(FEATURE_PCTRL_HAS_ALT_15) && (FEATURE_PCTRL_HAS_ALT_15 == 1)
    PCTRL_MUX_ALT8 = 8U,          /*!< Route the pin to alternate function 8. */
    PCTRL_MUX_ALT9 = 9U,          /*!< Route the pin to alternate function 9. */
    PCTRL_MUX_ALT10 = 10U,        /*!< Route the pin to alternate function 10. */
    PCTRL_MUX_ALT11 = 11U,        /*!< Route the pin to alternate function 11. */
    PCTRL_MUX_ALT12 = 12U,        /*!< Route the pin to alternate function 12. */
    PCTRL_MUX_ALT13 = 13U,        /*!< Route the pin to alternate function 13. */
    PCTRL_MUX_ALT14 = 14U,        /*!< Route the pin to alternate function 14. */
    PCTRL_MUX_ALT15 = 15U         /*!< Route the pin to alternate function 15. */
#endif
} port_mux_t;

/*!
 * @brief Interrupt or DMA trigger selection for a GPIO pin.
 *
 * The available trigger encodings depend on device feature macros. This
 * enumeration covers DMA trigger modes, interrupt modes, and optional
 * flag-only or trigger-out modes when implemented by the target device.
 */
typedef enum
{
    PCTRL_DMA_INT_DISABLED = 0x0U,    /*!< Disable pin interrupt and DMA trigger generation. */
    PCTRL_DMA_RISING_EDGE = 0x1U,     /*!< Generate a DMA request on a rising edge. */
    PCTRL_DMA_FALLING_EDGE = 0x2U,    /*!< Generate a DMA request on a falling edge. */
    PCTRL_DMA_EITHER_EDGE = 0x3U,     /*!< Generate a DMA request on either edge. */
#if FEATURE_PINS_HAS_FLAG_SET_ONLY
    PCTRL_FLAG_RISING_EDGE = 0x5U,    /*!< Set the interrupt flag on a rising edge only. */
    PCTRL_FLAG_FALLING_EDGE = 0x6U,   /*!< Set the interrupt flag on a falling edge only. */
    PCTRL_FLAG_EITHER_EDGE = 0x7U,    /*!< Set the interrupt flag on either edge only. */
#endif /* FEATURE_PINS_HAS_FLAG_SET_ONLY */
    PCTRL_INT_LOGIC_ZERO = 0x8U,      /*!< Generate an interrupt while the pin is at logic 0. */
    PCTRL_INT_RISING_EDGE = 0x9U,     /*!< Generate an interrupt on a rising edge. */
    PCTRL_INT_FALLING_EDGE = 0xAU,    /*!< Generate an interrupt on a falling edge. */
    PCTRL_INT_EITHER_EDGE = 0xBU,     /*!< Generate an interrupt on either edge. */
    PCTRL_INT_LOGIC_ONE = 0xCU,       /*!< Generate an interrupt while the pin is at logic 1. */
#if FEATURE_PINS_HAS_TRIGGER_OUT
    PCTRL_HIGH_TRIGGER_OUT = 0xDU,    /*!< Enable active-high trigger output without a flag. */
    PCTRL_LOW_TRIGGER_OUT = 0xEU      /*!< Enable active-low trigger output without a flag. */
#endif /* FEATURE_PINS_HAS_TRIGGER_OUT */
} gpio_interrupt_config_t;

#if FEATURE_PINS_HAS_SLEW_RATE
/*!
 * @brief Output slew rate selection.
 */
typedef enum
{
    PCTRL_FAST_SLEW_RATE = 0U,        /*!< Use the fast output slew rate. */
    PCTRL_SLOW_SLEW_RATE = 1U         /*!< Use the slow output slew rate. */
} port_slew_rate_t;
#endif /* FEATURE_PINS_HAS_SLEW_RATE */

#if defined(FEATURE_PINS_HAS_FILTER_CLOCK_SELECTION) && (FEATURE_PINS_HAS_FILTER_CLOCK_SELECTION == 1)
/*!
 * @brief Clock source selection for the GPIO digital filter.
 */
typedef enum
{
    PCTRL_DIGITAL_FILTER_BUS_CLOCK = 0U,  /*!< Clock the digital filter from the bus clock. */
    PCTRL_DIGITAL_FILTER_LPO_CLOCK = 1U   /*!< Clock the digital filter from the LPO clock. */
} port_digital_filter_clock_t;
#endif

/*!
 * @brief Digital filter configuration.
 *
 * Describes the optional debounce / glitch-filter configuration that can be
 * applied to a pin when the device exposes a digital filter block.
 */
typedef struct
{
#if defined(FEATURE_PINS_HAS_FILTER_CLOCK_SELECTION) && (FEATURE_PINS_HAS_FILTER_CLOCK_SELECTION == 1)
    port_digital_filter_clock_t clock;    /*!< Clock source used by the digital filter. */
#endif
    uint8_t width;                        /*!< Filter width programmed into the hardware. */
} gpio_digital_filter_config_t;

/*!
 * @brief Pin initialization record used by PINS_DRV_Init().
 *
 * Each array entry describes one pad and its optional GPIO behavior. Fields
 * guarded by `FEATURE_*` macros are only present on devices that support the
 * corresponding hardware capability.
 *
 * | Field         | Description |
 * |---------------|-------------|
 * | base          | PCTRL instance that owns the pad control register. |
 * | pinPortIdx    | Zero-based pin index inside the port instance. |
 * | mux           | Pad mux selection written to the PCR register. |
 * | intConfig     | Interrupt or DMA trigger mode programmed for the pin. |
 * | clearIntFlag  | Clears a pending interrupt flag after configuration when true. |
 * | digitalFilter | Enables the digital input filter after configuration when true. |
 * | filterConfig  | Filter width and optional clock source, when filter support exists. |
 * | gpioBase      | GPIO instance associated with the same pad. |
 * | direction     | GPIO direction used when `mux == PCTRL_MUX_AS_GPIO`. |
 * | initValue     | Initial output level written before enabling GPIO output. |
 */
typedef struct
{
    PCTRL_Type *base;                        /*!< Port control base pointer. */
    uint32_t pinPortIdx;                     /*!< Pin index inside the selected port. */
#if FEATURE_PINS_HAS_PULL_SELECTION
    port_pull_config_t pullConfig;           /*!< Internal pull resistor selection. */
#endif
#if FEATURE_PINS_HAS_SLEW_RATE
    port_slew_rate_t rateSelect;             /*!< Slew rate selection. */
#endif
#if FEATURE_PINS_HAS_PASSIVE_FILTER
    bool passiveFilter;                      /*!< Passive input filter enable state. */
#endif
#if FEATURE_PINS_HAS_OPEN_DRAIN
    port_open_drain_t openDrain;             /*!< Open-drain output enable state. */
#endif
#if FEATURE_PINS_HAS_DRIVE_STRENGTH
    port_drive_strength_t driveSelect;       /*!< Output drive strength selection. */
#endif
    port_mux_t mux;                          /*!< Pin mux selection. */
#if FEATURE_PCTRL_HAS_PIN_CONTROL_LOCK
    bool pinLock;                            /*!< Lock the PCR register after configuration. */
#endif
    gpio_interrupt_config_t intConfig;       /*!< Interrupt or DMA trigger configuration. */
    bool clearIntFlag;                       /*!< Clear the pin interrupt flag after setup. */
    bool digitalFilter;                      /*!< Enable the digital filter after setup. */
#if (FEATURE_PINS_HAS_DIGITAL_FILTER) || (FEATURE_PCTRL_HAS_DIGITAL_FILTER)
    gpio_digital_filter_config_t filterConfig; /*!< Digital filter configuration. */
#endif /* FEATURE_PINS_HAS_DIGITAL_FILTER || FEATURE_PCTRL_HAS_DIGITAL_FILTER */
    GPIO_Type *gpioBase;                     /*!< GPIO base pointer associated with the pin. */
    pin_direction_t direction;               /*!< GPIO direction used in GPIO mux mode. */
    pins_level_type_t initValue;             /*!< Initial GPIO output level. */
} pin_settings_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*******************************************************************************
 * Initialization
 ******************************************************************************/
/*!
 * @name Initialization
 * @brief Functions for applying one or more pin configuration records.
 * @{
 */

/*!
 * @brief Initialize a list of pins from an array of configuration records.
 *
 * Applies each entry in the `config` array in order by calling the internal
 * hardware access layer. A single record may configure mux selection,
 * electrical attributes, interrupt mode, GPIO direction, initial output level,
 * and optional digital filter behavior.
 *
 * @param[in] pinCount  Number of entries in the configuration array.
 * @param[in] config    Pointer to the first configuration record.
 * @return Execution status.
 * @retval STATUS_SUCCESS  All pin configurations were applied.
 *
 * @pre Each array entry must provide valid `base`, `gpioBase`, and pin index
 *      values compatible with the selected device.
 * @note When `mux` is `PCTRL_MUX_AS_GPIO`, `direction` must be either
 *       `GPIO_INPUT_DIRECTION` or `GPIO_OUTPUT_DIRECTION`.
 */
status_t PINS_DRV_Init(uint32_t pinCount,
                       const pin_settings_config_t config[]);

/*! @} */ /* End of Initialization */

/*******************************************************************************
 * Port Control
 ******************************************************************************/
/*!
 * @name Port Control
 * @brief Functions for updating pad mux and electrical attributes.
 * @{
 */

#if FEATURE_PINS_HAS_PULL_SELECTION
/*!
 * @brief Configure the internal pull resistor for a pin.
 *
 * Updates the pull-enable and pull-select bits of the selected PCR register.
 * The setting is meaningful for digital pin mux modes.
 *
 * @param[in] base        Port control base pointer.
 * @param[in] pin         Zero-based pin index inside the port.
 * @param[in] pullConfig  Internal pull resistor selection.
 */
void PINS_DRV_SetPullSel(PCTRL_Type *const base,
                         uint32_t pin,
                         port_pull_config_t pullConfig);
#endif /* FEATURE_PINS_HAS_PULL_SELECTION */

/*!
 * @brief Select the mux mode for a pin.
 *
 * Writes the mux field of the selected PCR register without changing the other
 * pin-control options already stored in that register.
 *
 * @param[in] base  Port control base pointer.
 * @param[in] pin   Zero-based pin index inside the port.
 * @param[in] mux   Mux selection to apply.
 */
void PINS_DRV_SetMuxModeSel(PCTRL_Type *const base,
                            uint32_t pin,
                            port_mux_t mux);

/*! @} */ /* End of Port Control */

/*******************************************************************************
 * Interrupt & Digital Filter Control
 ******************************************************************************/
/*!
 * @name Interrupt & Digital Filter Control
 * @brief Functions for configuring pin interrupt routing, flags, and optional
 *        digital filters.
 * @{
 */

/*!
 * @brief Configure the interrupt or DMA trigger mode of a pin.
 *
 * Programs the interrupt-control field associated with the selected pin.
 *
 * @param[in] base       GPIO base pointer.
 * @param[in] pin        Zero-based pin index inside the port.
 * @param[in] intConfig  Interrupt or DMA trigger configuration.
 */
void PINS_DRV_SetPinIntSel(GPIO_Type *const base,
                           uint32_t pin,
                           gpio_interrupt_config_t intConfig);

/*!
 * @brief Read the current interrupt or DMA trigger configuration of a pin.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pin   Zero-based pin index inside the port.
 * @return Current interrupt or DMA trigger configuration.
 */
gpio_interrupt_config_t PINS_DRV_GetPinIntSel(const GPIO_Type *const base,
                                              uint32_t pin);

/*!
 * @brief Clear the interrupt status flag of one pin.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pin   Zero-based pin index inside the port.
 */
void PINS_DRV_ClearPinIntFlagCmd(GPIO_Type *const base,
                                 uint32_t pin);

/*!
 * @brief Enable the digital filter for one pin.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pin   Zero-based pin index inside the port.
 *
 * @note This API is only meaningful on devices that implement a GPIO digital
 *       filter block.
 */
void PINS_DRV_EnableDigitalFilter(GPIO_Type *const base,
                                  uint32_t pin);

/*!
 * @brief Disable the digital filter for one pin.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pin   Zero-based pin index inside the port.
 */
void PINS_DRV_DisableDigitalFilter(GPIO_Type *const base,
                                   uint32_t pin);

/*!
 * @brief Program the digital filter settings for one pin.
 *
 * Updates the filter width and, when supported, the filter clock source of
 * the selected pin.
 *
 * @param[in] base    GPIO base pointer.
 * @param[in] config  Pointer to the digital filter configuration structure.
 * @param[in] pin     Zero-based pin index inside the port.
 *
 * @note Update the filter parameters only while the corresponding filter is
 *       disabled.
 */
void PINS_DRV_ConfigDigitalFilter(GPIO_Type *const base,
                                  const gpio_digital_filter_config_t *const config,
                                  uint32_t pin);

/*!
 * @brief Read the interrupt status flags for an entire GPIO port.
 *
 * @param[in] base  GPIO base pointer.
 * @return Bit mask of pending interrupt flags for all pins in the port.
 */
uint32_t PINS_DRV_GetPortIntFlag(const GPIO_Type *const base);

/*!
 * @brief Clear all interrupt status flags for a GPIO port.
 *
 * @param[in] base  GPIO base pointer.
 */
void PINS_DRV_ClearPortIntFlagCmd(GPIO_Type *const base);

/*! @} */ /* End of Interrupt & Digital Filter Control */

/*******************************************************************************
 * GPIO Direction Control
 ******************************************************************************/
/*!
 * @name GPIO Direction Control
 * @brief Functions for managing GPIO direction and optional input-disable
 *        behavior.
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
 * @return Current direction mask for the entire port.
 */
pins_channel_type_t PINS_DRV_GetPinsDirection(const GPIO_Type *const base);

/*!
 * @brief Set the direction of one GPIO pin.
 *
 * @param[in] base       GPIO base pointer.
 * @param[in] pin        Zero-based pin index inside the port.
 * @param[in] direction  Direction to apply to the selected pin.
 *
 * @note On devices that support input-disable control, call
 *       PINS_DRV_SetPortInputDisable() separately when output-only behavior is
 *       required.
 */
void PINS_DRV_SetPinDirection(GPIO_Type *const base,
                              pins_channel_type_t pin,
                              pin_direction_t direction);

/*!
 * @brief Set the direction mask of an entire GPIO port.
 *
 * Each bit in `pins` corresponds to one pin:
 *   - `0`: configure as input
 *   - `1`: configure as output
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pins  Direction mask to write to the port.
 */
void PINS_DRV_SetPinsDirection(GPIO_Type *const base,
                               pins_channel_type_t pins);

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
void PINS_DRV_SetPortInputDisable(GPIO_Type *const base,
                                  pins_channel_type_t pins);

/*!
 * @brief Read the input-disable mask of a GPIO port.
 *
 * @param[in] base  GPIO base pointer.
 * @return Current input-disable mask for the port.
 */
pins_channel_type_t PINS_DRV_GetPortInputDisable(const GPIO_Type *const base);
#endif /* FEATURE_PINS_HAS_INPUT_DISABLE */

/*! @} */ /* End of GPIO Direction Control */

/*******************************************************************************
 * GPIO Data Access
 ******************************************************************************/
/*!
 * @name GPIO Data Access
 * @brief Functions for reading and driving GPIO output values.
 * @{
 */

/*!
 * @brief Write one GPIO pin with a logic level.
 *
 * @param[in] base   GPIO base pointer.
 * @param[in] pin    Zero-based pin index inside the port.
 * @param[in] value  Logic level to drive on the selected pin.
 */
void PINS_DRV_WritePin(GPIO_Type *const base,
                       pins_channel_type_t pin,
                       pins_level_type_t value);

/*!
 * @brief Write the output register of an entire GPIO port.
 *
 * Each bit in `pins` corresponds to one GPIO output:
 *   - `0`: drive logic low
 *   - `1`: drive logic high
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pins  Output value mask to write.
 */
void PINS_DRV_WritePins(GPIO_Type *const base,
                        pins_channel_type_t pins);

/*!
 * @brief Read the current output register value of a GPIO port.
 *
 * @param[in] base  GPIO base pointer.
 * @return Current output latch value for the port.
 */
pins_channel_type_t PINS_DRV_GetPinsOutput(const GPIO_Type *const base);

/*!
 * @brief Set selected GPIO output pins to logic high.
 *
 * Bits set to `1` in `pins` are driven high. Bits cleared to `0` are left
 * unchanged.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pins  Mask of pins to set.
 */
void PINS_DRV_SetPins(GPIO_Type *const base,
                      pins_channel_type_t pins);

/*!
 * @brief Clear selected GPIO output pins to logic low.
 *
 * Bits set to `1` in `pins` are driven low. Bits cleared to `0` are left
 * unchanged.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pins  Mask of pins to clear.
 */
void PINS_DRV_ClearPins(GPIO_Type *const base,
                        pins_channel_type_t pins);

/*!
 * @brief Toggle selected GPIO output pins.
 *
 * Bits set to `1` in `pins` are toggled. Bits cleared to `0` are left
 * unchanged.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pins  Mask of pins to toggle.
 */
void PINS_DRV_TogglePins(GPIO_Type *const base,
                         pins_channel_type_t pins);

/*!
 * @brief Read the input value of an entire GPIO port.
 *
 * @param[in] base  GPIO base pointer.
 * @return Current sampled input levels for the port.
 */
pins_channel_type_t PINS_DRV_ReadPins(const GPIO_Type *const base);

/*!
 * @brief Read the input value of one GPIO pin.
 *
 * @param[in] base  GPIO base pointer.
 * @param[in] pin   Zero-based pin index inside the port.
 * @return Logic level sampled on the selected pin.
 */
pins_level_type_t PINS_DRV_ReadPin(const GPIO_Type * const base,
                                   pins_channel_type_t pin);

/*! @} */ /* End of GPIO Data Access */

#if defined(__cplusplus)
}
#endif

/*! @} */ /* End of pins group */

#endif /* PINS_DRIVER_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

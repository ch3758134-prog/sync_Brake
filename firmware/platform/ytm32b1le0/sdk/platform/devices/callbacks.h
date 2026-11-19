/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file callbacks.h
 * @version 1.4.1
 * @brief Shared callback enums and function signatures for platform/devices.
 */

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <stdint.h>

/*!
 * @addtogroup devices_callbacks
 * @brief Callback contracts shared by multiple peripheral driver modules.
 * @details
 * This header centralizes event enums and callback typedefs that are reused by
 * I2C, SPI, UART, ADC, CAN, timer, output-compare, and input-capture drivers.
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @name I2C Callback Types
 * @brief Event enums and callback signatures shared by I2C master and slave APIs.
 * @{
 */

/*!
 * @brief I2C slave callback event identifiers.
 *
 * | Value | Description |
 * |-------|-------------|
 * | `I2C_SLAVE_EVENT_RX_FULL` | The slave receive path reached its configured full condition. |
 * | `I2C_SLAVE_EVENT_TX_EMPTY` | The slave transmit path needs more data. |
 * | `I2C_SLAVE_EVENT_TX_REQ` | The master requested slave transmit service. |
 * | `I2C_SLAVE_EVENT_RX_REQ` | The master requested slave receive service. |
 * | `I2C_SLAVE_EVENT_STOP` | A STOP condition ended the current slave transaction. |
 */
typedef enum
{
    I2C_SLAVE_EVENT_RX_FULL = 0x00U,
    I2C_SLAVE_EVENT_TX_EMPTY = 0x01U,
    I2C_SLAVE_EVENT_TX_REQ = 0x02U,
    I2C_SLAVE_EVENT_RX_REQ = 0x03U,
    I2C_SLAVE_EVENT_STOP = 0X04U,
} i2c_slave_event_t;

/*!
 * @brief I2C master callback event identifiers.
 *
 * | Value | Description |
 * |-------|-------------|
 * | `I2C_MASTER_EVENT_TX_END` | The configured transmit sequence finished. |
 * | `I2C_MASTER_EVENT_RX_END` | The configured receive sequence finished. |
 * | `I2C_MASTER_EVENT_FIFO_ERROR_REQ` | A FIFO service or FIFO error condition was reported. |
 * | `I2C_MASTER_EVENT_LINE_LOW_TIMEOUT` | SCL or SDA remained low beyond the timeout window. |
 * | `I2C_MASTER_EVENT_ARBITRATION_LOST_ERROR` | The master lost arbitration on the bus. |
 * | `I2C_MASTER_EVENT_NACK_DETECT` | The addressed target responded with NACK. |
 * | `I2C_MASTER_EVENT_END_TRANSFER` | The transfer reached its terminal completion state. |
 */
typedef enum
{
    I2C_MASTER_EVENT_TX_END                            = 0x0U,
    I2C_MASTER_EVENT_RX_END                            = 0x1U,
    I2C_MASTER_EVENT_FIFO_ERROR_REQ                    = 0x2U,
    I2C_MASTER_EVENT_LINE_LOW_TIMEOUT                  = 0x3U,
    I2C_MASTER_EVENT_ARBITRATION_LOST_ERROR            = 0x4U,
    I2C_MASTER_EVENT_NACK_DETECT                       = 0x5U,
    I2C_MASTER_EVENT_END_TRANSFER                      = 0x6U,
} i2c_master_event_t;

/*!
 * @brief I2C slave callback function signature.
 *
 * @param[in] event     Slave-side event that triggered the callback.
 * @param[in] userData  User pointer supplied during driver configuration.
 */
typedef void (*i2c_slave_callback_t)(i2c_slave_event_t event, void *userData);

/*!
 * @brief I2C master callback function signature.
 *
 * @param[in] event     Master-side event that triggered the callback.
 * @param[in] userData  User pointer supplied during driver configuration.
 */
typedef void (*i2c_master_callback_t)(i2c_master_event_t event, void *userData);

/*! @} */ /* End of I2C Callback Types */

/*!
 * @name SPI Callback Types
 * @brief Event enums and callback signatures shared by SPI drivers.
 * @{
 */

/*!
 * @brief SPI callback event identifiers.
 *
 * | Value | Description |
 * |-------|-------------|
 * | `SPI_EVENT_END_TRANSFER` | The programmed SPI transfer completed. |
 */
typedef enum
{
    SPI_EVENT_END_TRANSFER = 0
} spi_event_t;

/*!
 * @brief SPI callback function signature.
 *
 * @param[in] driverState  Driver-specific runtime state pointer.
 * @param[in] event        SPI event that triggered the callback.
 * @param[in] userData     User pointer supplied during driver configuration.
 */
typedef void (*spi_callback_t)(void *driverState, spi_event_t event, void *userData);

/*! @} */ /* End of SPI Callback Types */

/*!
 * @name UART Callback Types
 * @brief Event enums and callback signatures shared by UART-family drivers.
 * @{
 */

/*!
 * @brief UART callback event identifiers.
 *
 * | Value | Description |
 * |-------|-------------|
 * | `UART_EVENT_RX_FULL` | The receive buffer reached its configured limit. |
 * | `UART_EVENT_TX_EMPTY` | The transmit buffer needs more data. |
 * | `UART_EVENT_END_TRANSFER` | The active transfer completed. |
 * | `UART_EVENT_ERROR` | A transfer error was detected. |
 * | `UART_EVENT_TIMEOUT` | A UART timeout condition was detected. |
 */
typedef enum
{
    UART_EVENT_RX_FULL      = 0x00U,
    UART_EVENT_TX_EMPTY     = 0x01U,
    UART_EVENT_END_TRANSFER = 0x02U,
    UART_EVENT_ERROR        = 0x03U,
    UART_EVENT_TIMEOUT      = 0x04U,
} uart_event_t;

/*!
 * @brief UART callback function signature.
 *
 * @param[in] driverState  Driver-specific runtime state pointer.
 * @param[in] event        UART event that triggered the callback.
 * @param[in] userData     User pointer supplied during driver configuration.
 */
typedef void (*uart_callback_t)(void *driverState, uart_event_t event, void *userData);

/*! @} */ /* End of UART Callback Types */

/*!
 * @name Timer and ADC Callback Types
 * @brief Shared callback contracts for timer-style and ADC driver flows.
 * @{
 */

/*!
 * @brief Generic timer callback function signature.
 *
 * @param[in] userData  User pointer supplied during driver configuration.
 */
typedef void (*timer_callback_t)(void *userData);

/*!
 * @brief ADC callback context passed back to the application layer.
 *
 * | Field | Description |
 * |-------|-------------|
 * | `groupIndex` | ADC conversion group that produced the callback. |
 * | `resultBufferTail` | Offset of the most recent result inside the result buffer. |
 */
typedef struct
{
    uint32_t groupIndex;         /*!< ADC conversion group that triggered the callback. */
    uint16_t resultBufferTail;   /*!< Offset of the newest conversion result in the result buffer. */
} adc_callback_info_t;

/*!
 * @brief ADC callback function signature.
 *
 * @param[in] callbackInfo  ADC callback context for the completed group.
 * @param[in] userData      User pointer supplied during driver configuration.
 */
typedef void (* const adc_callback_t)(const adc_callback_info_t * const callbackInfo, void * userData);

/*! @} */ /* End of Timer and ADC Callback Types */

/*!
 * @name CAN and Security Callback Types
 * @brief Shared callback contracts for CAN and security-oriented peripherals.
 * @{
 */

/*!
 * @brief CAN callback event identifiers.
 *
 * | Value | Description |
 * |-------|-------------|
 * | `CAN_EVENT_RX_COMPLETE` | A frame was received into the configured RX object. |
 * | `CAN_EVENT_TX_COMPLETE` | A frame was transmitted from the configured TX object. |
 * | `CAN_EVENT_SELF_WAKEUP` | The controller reported a self-wakeup event. |
 */
typedef enum {
    CAN_EVENT_RX_COMPLETE,
    CAN_EVENT_TX_COMPLETE,
    CAN_EVENT_SELF_WAKEUP
} can_event_t;

/*!
 * @brief CAN callback function signature.
 *
 * @param[in] instance     CAN controller instance index.
 * @param[in] eventType    Event that triggered the callback.
 * @param[in] objIdx       Buffer, mailbox, or object index associated with the event.
 * @param[in] driverState  Driver-specific runtime state pointer.
 */
typedef void (*can_callback_t)(uint32_t instance,
                               can_event_t eventType,
                               uint32_t objIdx,
                               void *driverState);

/*!
 * @brief Security-module callback function signature.
 *
 * @param[in] completedCmd   Identifier of the completed command.
 * @param[in] callbackParam  User pointer supplied during driver configuration.
 */
typedef void (*security_callback_t)(uint32_t completedCmd, void *callbackParam);

/*! @} */ /* End of CAN and Security Callback Types */

/*!
 * @name Capture and Compare Callback Types
 * @brief Event enums and callback signatures for OC and IC services.
 * @{
 */

/*!
 * @brief Output-compare callback event identifiers.
 *
 * | Value | Description |
 * |-------|-------------|
 * | `OC_EVENT_GENERATION_OUTPUT_COMPLETE` | Generation output signal is completed. |
 */
typedef enum
{
    OC_EVENT_GENERATION_OUTPUT_COMPLETE = 0x00U
} oc_event_t;

/*!
 * @brief Output-compare callback function signature.
 *
 * @param[in] event     Output-compare event that triggered the callback.
 * @param[in] userData  User pointer supplied during driver configuration.
 */
typedef void (*oc_callback_t)(oc_event_t event, void *userData);

/*!
 * @brief Input-capture callback event identifiers.
 *
 * | Value | Description |
 * |-------|-------------|
 * | `IC_EVENT_MEASUREMENT_COMPLETE` | The configured capture measurement completed. |
 */
typedef enum
{
    IC_EVENT_MEASUREMENT_COMPLETE = 0x00U
} ic_event_t;

/*!
 * @brief Input-capture callback function signature.
 *
 * @param[in] event     Input-capture event that triggered the callback.
 * @param[in] userData  User pointer supplied during driver configuration.
 */
typedef void (*ic_callback_t)(ic_event_t event, void *userData);

/*! @} */ /* End of Capture and Compare Callback Types */

/*! @} */ /* End of devices_callbacks */

#endif /* CALLBACKS_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/

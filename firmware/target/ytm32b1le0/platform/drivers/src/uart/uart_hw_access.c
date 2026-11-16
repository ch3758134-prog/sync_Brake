/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file uart_hw_access.c
 * @brief UART hardware access layer - non-inline function implementations.
 *
 * This file provides the register-level functions that are too complex to
 * inline (multi-step configuration, iterative flag handling, etc.).
 * All functions operate directly on the UART peripheral registers through
 * the base pointer.
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 2985 Rule 2.2: This operation is redundant. The value of the result is 
 *                       always that of the left-hand operand.
 */

#include "uart_hw_access.h"

/*******************************************************************************
 * Frame Format Configuration
 ******************************************************************************/

/*!
 * @brief Configure the number of data bits per character.
 *
 * When parity is enabled the hardware frame size is incremented by one to
 * accommodate the parity bit.  Disable the transmitter/receiver before
 * calling this function to ensure safe operation.
 *
 * @param[in] base             UART peripheral base pointer.
 * @param[in] bitCountPerChar  Desired data-bit width (7, 8, 9, or 10).
 * @param[in] parity           true if parity is enabled (adds one bit to frame).
 */
void UART_SetBitCountPerChar(UART_Type * base, uart_bit_count_per_char_t bitCountPerChar, bool parity)
{
    uint32_t tmpBitCountPerChar = (uint32_t)bitCountPerChar;
    if (parity)
    {
        tmpBitCountPerChar += 1U;
    }

    base->CTRL1 &= ~UART_CTRL1_FRAMESZ_MASK;
    base->CTRL1 |= (tmpBitCountPerChar << UART_CTRL1_FRAMESZ_SHIFT);
}

/*!
 * @brief Configure the parity mode.
 *
 * Sets parity enable and parity type (odd/even) fields in CTRL1.
 * Disable the transmitter/receiver before calling this function.
 *
 * @param[in] base            UART peripheral base pointer.
 * @param[in] parityModeType  Desired parity mode (disabled / even / odd).
 */
void UART_SetParityMode(UART_Type * base, uart_parity_mode_t parityModeType)
{
    base->CTRL1 = (base->CTRL1 & ~UART_CTRL1_PAREN_MASK) | (((uint32_t)parityModeType >> 1U) << UART_CTRL1_PAREN_SHIFT);
    base->CTRL1 = (base->CTRL1 & ~UART_CTRL1_PARSEL_MASK) | (((uint32_t)parityModeType & 1U) << UART_CTRL1_PARSEL_SHIFT); /* PRQA S 2985 */
}

/*******************************************************************************
 * Data Transfer
 ******************************************************************************/

/*!
 * @brief Send a 9-bit data character.
 *
 * Writes the lower 9 bits of @a data into the DATA register for transmission.
 *
 * @param[in] base  UART peripheral base pointer.
 * @param[in] data  9-bit data value to transmit.
 */
void UART_Putchar9(UART_Type * base, uint16_t data)
{
    base->DATA = data;
}

/*!
 * @brief Send a 10-bit data character.
 *
 * Writes the lower 10 bits of @a data into the DATA register for transmission.
 *
 * @param[in] base  UART peripheral base pointer.
 * @param[in] data  10-bit data value to transmit.
 */
void UART_Putchar10(UART_Type * base, uint16_t data)
{
    base->DATA = data;
}

/*!
 * @brief Receive a 7-bit data character.
 *
 * Reads the DATA register and masks the result to the lower 7 bits.
 *
 * @param[in]  base      UART peripheral base pointer.
 * @param[out] readData  Pointer to store the received 7-bit value.
 */
void UART_Getchar7(const UART_Type * base, uint8_t *readData)
{
    DEV_ASSERT(readData != NULL);

    *readData = (uint8_t)(base->DATA & 0x7FU);
}

/*!
 * @brief Receive an 8-bit data character.
 *
 * Reads the DATA register and stores the lower 8 bits.
 *
 * @param[in]  base      UART peripheral base pointer.
 * @param[out] readData  Pointer to store the received 8-bit value.
 */
void UART_Getchar8(const UART_Type * base, uint8_t *readData)
{
    DEV_ASSERT(readData != NULL);

    *readData = (uint8_t)base->DATA;
}

/*!
 * @brief Receive a 9-bit data character.
 *
 * Reads the DATA register and masks the result to the lower 9 bits.
 *
 * @param[in]  base      UART peripheral base pointer.
 * @param[out] readData  Pointer to store the received 9-bit value.
 */
void UART_Getchar9(const UART_Type * base, uint16_t *readData)
{
    DEV_ASSERT(readData != NULL);

    /* get data from the uart data register */
    *readData = (uint16_t)(base->DATA & 0x1FFU);
}

/*!
 * @brief Receive a 10-bit data character.
 *
 * Reads the DATA register and masks the result to the lower 10 bits.
 *
 * @param[in]  base      UART peripheral base pointer.
 * @param[out] readData  Pointer to store the received 10-bit value.
 */
void UART_Getchar10(const UART_Type * base, uint16_t *readData)
{
    DEV_ASSERT(readData != NULL);

    /* get data */
    *readData = (uint16_t)(base->DATA & 0x3FFU);
}

/*******************************************************************************
 * Interrupt and DMA
 ******************************************************************************/

/*!
 * @brief Enable or disable a specific UART interrupt source.
 *
 * Sets or clears the corresponding bit in the INTE register.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] intSrc  Interrupt source to configure (see @ref uart_interrupt_t).
 * @param[in] enable  true = enable, false = disable.
 */
void UART_SetIntMode(UART_Type * base, uart_interrupt_t intSrc, bool enable)
{
    SDK_ENTER_CRITICAL();
    if (enable){
        base->INTE |= (uint32_t)intSrc;
    } else {
        base->INTE &= ~(uint32_t)intSrc;
    }
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Query whether a specific UART interrupt source is enabled.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] intSrc  Interrupt source to query (see @ref uart_interrupt_t).
 * @return true if the interrupt source is enabled, false otherwise.
 */
bool UART_GetIntMode(const UART_Type * base, uart_interrupt_t intSrc)
{
    return (base->INTE & (uint32_t)intSrc) != 0U;
}

/*!
 * @brief Enable or disable all error-related interrupts at once.
 *
 * Controls parity-error, noise-error, framing-error, and overrun interrupts
 * in a single call.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] enable  true = enable all error interrupts, false = disable.
 */
void UART_SetErrorInterrupts(UART_Type * base, bool enable)
{
    /* Configure the error interrupts */
    UART_SetIntMode(base, UART_INT_RX_OVERRUN, enable);
    UART_SetIntMode(base, UART_INT_PARITY_ERR_FLAG, enable);
    UART_SetIntMode(base, UART_INT_NOISE_ERR_FLAG, enable);
    UART_SetIntMode(base, UART_INT_FRAME_ERR_FLAG, enable);
}

/*******************************************************************************
 * Status Flags
 ******************************************************************************/

/*!
 * @brief Read the state of a UART status flag.
 *
 * @param[in] base        UART peripheral base pointer.
 * @param[in] statusFlag  Flag to query (see @ref uart_status_flag_t).
 * @return true if the flag is set, false otherwise.
 */
bool UART_GetStatusFlag(const UART_Type * base, uart_status_flag_t statusFlag)
{
    return (base->INTF & (uint32_t)statusFlag) != 0U;
}

/*!
 * @brief Clear a UART status flag.
 *
 * Writes the flag mask to the INTF register (w1c behavior).
 *
 * @param[in] base        UART peripheral base pointer.
 * @param[in] statusFlag  Flag to clear (see @ref uart_status_flag_t).
 * @return STATUS_SUCCESS always.
 */
status_t UART_ClearStatusFlag(UART_Type * base,
                                    uart_status_flag_t statusFlag)
{
    status_t returnCode = STATUS_SUCCESS;
    base->INTF = (uint32_t)statusFlag;
    return (returnCode);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/

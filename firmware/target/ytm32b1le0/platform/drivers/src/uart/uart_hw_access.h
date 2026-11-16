/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file uart_hw_access.h
 * @brief UART hardware register access - inline helpers and non-inline declarations.
 *
 * Provides direct register-level operations grouped by function:
 * Initialization, TX/RX Control, Baud Rate, Frame Format, Interrupt / DMA,
 * Data Transfer, Status Flags, FIFO, Modem (CTS/RTS), and One-Wire Mode.
 */

#ifndef UART_HW_ACCESS_H__
#define UART_HW_ACCESS_H__

#include "uart_driver.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define UART_SHIFT          (16U)
#define UART_BAUD_REG_ID    (1U)
#define UART_CTRL0_REG_ID   (2U)
#define UART_CTRL1_REG_ID   (3U)
#define UART_STATUTS_REG_ID (4U)
#define UART_INTE_REG_ID      (5U)
#define UART_MATCH_REG_ID   (6U)
#define UART_DATA_REG_ID    (7U)
#define UART_TXFIFO_REG_ID  (8U)
#define UART_RXFIFO_REG_ID  (9U)

/*! @brief UART wakeup from standby method constants
 */
typedef enum
{
    UART_IDLE_LINE_WAKE = 0x0U, /*!< Idle-line wakes the UART receiver from standby. */
    UART_ADDR_MARK_WAKE = 0x1U  /*!< Addr-mark wakes UART receiver from standby.*/
} uart_wakeup_method_t;

/*!
 * @brief UART break character length settings for transmit/detect.
 *
 * The actual maximum bit times may vary depending on the UART instance.
 */
typedef enum
{
    UART_BREAK_CHAR_10_BIT_MINIMUM = 0x0U, /*!< UART break char length 10 bit times (if M = 0, SBNS = 0)
                                                  or 11 (if M = 1, SBNS = 0 or M = 0, SBNS = 1) or 12 (if M = 1,
                                                  SBNS = 1 or M10 = 1, SNBS = 0) or 13 (if M10 = 1, SNBS = 1) */
    UART_BREAK_CHAR_13_BIT_MINIMUM = 0x1U  /*!< UART break char length 13 bit times (if M = 0, SBNS = 0
                                                  or M10 = 0, SBNS = 1) or 14 (if M = 1, SBNS = 0 or M = 1,
                                                  SBNS = 1) or 15 (if M10 = 1, SBNS = 1 or M10 = 1, SNBS = 0) */
} uart_break_char_length_t;

/*!
 * @brief UART status flags.
 *
 * This provides constants for the UART status flags for use in the UART functions.
 */
typedef enum
{
    UART_TX_DATA_REG_EMPTY          = UART_INTF_TXIF_MASK,       /*!< Tx data register empty flag, sets when Tx buffer is empty */
    UART_TX_COMPLETE                = UART_INTF_TCIF_MASK,       /*!< Transmission complete flag, sets when transmission activity complete */
    UART_RX_DATA_REG_FULL           = UART_INTF_RXIF_MASK,       /*!< Rx data register full flag, sets when the receive data buffer is full */
    UART_IDLE_LINE_DETECT           = UART_INTF_IDLEIF_MASK,     /*!< Idle line detect flag, sets when idle line detected */
    UART_RX_OVERRUN                 = UART_INTF_OVRIF_MASK,      /*!< Rx Overrun sets if new data is received before data is read */
    UART_NOISE_DETECT               = UART_INTF_NOZIF_MASK,      /*!< Rx takes 3 samples of each received bit. If these differ, the flag sets */
    UART_FRAME_ERR                  = UART_INTF_FEIF_MASK,       /*!< Frame error flag, sets if logic 0 was detected where stop bit expected */
    UART_PARITY_ERR                 = UART_INTF_PARIF_MASK,      /*!< If parity enabled, sets upon parity error detection */
    UART_LIN_BREAK_DETECT           = UART_INTF_LBKDIF_MASK,     /*!< LIN break detect interrupt flag, sets when LIN break char detected */
    UART_RX_ACTIVE_EDGE_DETECT      = UART_INTF_RXEDGEIF_MASK,   /*!< Rx pin active edge interrupt flag, sets when active edge detected */
#if FEATURE_UART_HAS_ADDRESS_MATCHING
    UART_MATCH_ADDR_ONE             = UART_INTF_MATIF_MASK,      /*!< Address one match flag */
#endif
#if FEATURE_UART_FIFO_SIZE > 0U
    UART_FIFO_TX_OF                 = UART_INTF_TFEIF_MASK,      /*!< Transmitter FIFO buffer overflow */
    UART_FIFO_RX_UF                 = UART_INTF_RFEIF_MASK,      /*!< Receiver FIFO buffer underflow */
#endif
} uart_status_flag_t;

/*! @brief UART interrupt configuration structure, default settings are 0 (disabled) */
typedef enum
{
    UART_INT_LIN_BREAK_DETECT  = UART_INTE_LBKDIE_MASK,               /*!< LIN break detect. */
    UART_INT_RX_ACTIVE_EDGE    = UART_INTE_RXEDGEIE_MASK,             /*!< RX Active Edge. */
    UART_INT_TX_DATA_REG_EMPTY = UART_INTE_TXIE_MASK,                 /*!< Transmit data register empty. */
    UART_INT_TX_COMPLETE       = UART_INTE_TCIE_MASK,                 /*!< Transmission complete. */
    UART_INT_RX_DATA_REG_FULL  = UART_INTE_RXIE_MASK,                 /*!< Receiver data register full. */
    UART_INT_IDLE_LINE         = UART_INTE_IDLEIE_MASK,               /*!< Idle line. */
    UART_INT_RX_OVERRUN        = UART_INTE_OVRIE_MASK,                /*!< Receiver Overrun. */
    UART_INT_NOISE_ERR_FLAG    = UART_INTE_NOZIE_MASK,                /*!< Noise error flag. */
    UART_INT_FRAME_ERR_FLAG    = UART_INTE_FEIE_MASK,                 /*!< Framing error flag. */
    UART_INT_PARITY_ERR_FLAG   = UART_INTE_PARIE_MASK,                /*!< Parity error flag. */
#if FEATURE_UART_HAS_ADDRESS_MATCHING
    UART_INT_MATCH_ADDR_ONE    = UART_INTE_MATIE_MASK,                /*!< Match address one flag. */
#endif
} uart_interrupt_t;


/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name TX/RX Control and Frame Format
 * @{
 */

/*!
 * @brief Enable or disable the UART transmitter.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] enable  true = enable, false = disable.
 */
static inline void UART_SetTransmitterCmd(UART_Type * base, bool enable)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 = (base->CTRL0 & ~UART_CTRL0_TXEN_MASK) | ((enable ? 1UL : 0UL) << UART_CTRL0_TXEN_SHIFT);
    /* Wait for the register write operation to complete */
    while((bool)((base->CTRL0 & UART_CTRL0_TXEN_MASK) != 0U) != enable) {}
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Enable or disable the UART receiver.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] enable  true = enable, false = disable.
 */
static inline void UART_SetReceiverCmd(UART_Type * base, bool enable)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 = (base->CTRL0 & ~UART_CTRL0_RXEN_MASK) | ((enable ? 1UL : 0UL) << UART_CTRL0_RXEN_SHIFT);
    /* Wait for the register write operation to complete */
    while((bool)((base->CTRL0 & UART_CTRL0_RXEN_MASK) != 0U) != enable) {}
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Set the Baud Rate Divisor (SBR).
 *
 * @param[in] base              UART peripheral base pointer.
 * @param[in] baudRateDivisor   SBR value (1-0x1FFF).
 */
static inline void UART_SetBaudRateDivisor(UART_Type * base, uint32_t baudRateDivisor)
{
    DEV_ASSERT((baudRateDivisor <= 0x1FFFU) && (baudRateDivisor >= 1U));
    uint32_t baudRegValTemp;

    baudRegValTemp = base->BAUD;
    baudRegValTemp &= ~(UART_BAUD_DIV_MASK);
    /* Removed the shift operation as the SBR field position is zero; shifting with 0 violates MISRA */
    baudRegValTemp |= baudRateDivisor & UART_BAUD_DIV_MASK;
    base->BAUD = baudRegValTemp;
}

/*!
 * @brief Read back the current Baud Rate Divisor (SBR).
 *
 * @param[in] base  UART peripheral base pointer.
 * @return Current SBR value.
 */
static inline uint16_t UART_GetBaudRateDivisor(const UART_Type * base)
{
    return ((uint16_t)((base->BAUD & UART_BAUD_DIV_MASK) >> UART_BAUD_DIV_SHIFT));
}

#if FEATURE_UART_HAS_BAUD_RATE_OVER_SAMPLING_SUPPORT
/*!
 * @brief Set the Over-Sampling Ratio (OSR).
 *
 * Valid range: 4x (0x03) - 32x (0x1F).  Disable the transmitter/receiver
 * before calling.
 *
 * @param[in] base                UART peripheral base pointer.
 * @param[in] overSamplingRatio   OSR value (bit-value = actual ratio - 1).
 */
static inline void UART_SetOversamplingRatio(UART_Type * base, uint32_t overSamplingRatio)
{
    DEV_ASSERT(overSamplingRatio <= 0x1FU);
    uint32_t baudRegValTemp;

    baudRegValTemp = base->BAUD;
    baudRegValTemp &= ~(UART_BAUD_OSRVAL_MASK);
    baudRegValTemp |= UART_BAUD_OSRVAL(overSamplingRatio);
    base->BAUD = baudRegValTemp;
}

/*!
 * @brief Read back the current Over-Sampling Ratio (OSR).
 *
 * @param[in] base  UART peripheral base pointer.
 * @return Current OSR value (bit-value; actual ratio = value + 1).
 */
static inline uint8_t UART_GetOversamplingRatio(const UART_Type * base)
{
    return ((uint8_t)((base->BAUD & UART_BAUD_OSRVAL_MASK) >> UART_BAUD_OSRVAL_SHIFT));
}
#endif

#if FEATURE_UART_HAS_BOTH_EDGE_SAMPLING_SUPPORT
/*!
 * @brief Enable both-edge sampling for the baud-rate clock.
 *
 * Must be enabled when the OSR is between 4x and 7x.  Call only when the
 * receiver is disabled.
 *
 * @param[in] base  UART peripheral base pointer.
 */
static inline void UART_EnableBothEdgeSamplingCmd(UART_Type * base)
{
    base->CTRL1 |= UART_CTRL1_BOTHEDGE_MASK;
}
#endif

/*!
 * @brief Configure the number of bits per character.
 *
 * Adjusts M and M10 bits.  Disable TX/RX before calling.
 *
 * @param[in] base            UART peripheral base pointer.
 * @param[in] bitCountPerChar Character width (7, 8, 9, or 10 bits).
 * @param[in] parity          true if a parity bit is used.
 */
void UART_SetBitCountPerChar(UART_Type * base, uart_bit_count_per_char_t bitCountPerChar, bool parity);

/*!
 * @brief Configure the parity mode.
 *
 * Disable TX/RX before calling.
 *
 * @param[in] base           UART peripheral base pointer.
 * @param[in] parityModeType Parity setting (disabled / even / odd).
 */
void UART_SetParityMode(UART_Type * base, uart_parity_mode_t parityModeType);

/*!
 * @brief Configure the number of stop bits (1 or 2).
 *
 * @param[in] base          UART peripheral base pointer.
 * @param[in] stopBitCount  Stop-bit count selector.
 */
static inline void UART_SetStopBitCount(UART_Type * base, uart_stop_bit_count_t stopBitCount)
{
    base->CTRL1 = (base->CTRL1 & ~UART_CTRL1_STOPSZ_MASK) | ((uint32_t)stopBitCount << UART_CTRL1_STOPSZ_SHIFT);
}

/*@}*/

/*!
 * @name Interrupt and DMA
 * @{
 */

/*!
 * @brief Enable or disable an individual UART interrupt source.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] intSrc  Interrupt source selector.
 * @param[in] enable  true = enable, false = disable.
 */
void UART_SetIntMode(UART_Type * base, uart_interrupt_t intSrc, bool enable);

/*!
 * @brief Query whether an interrupt source is currently enabled.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] intSrc  Interrupt source selector.
 * @return true if enabled, false if disabled.
 */
bool UART_GetIntMode(const UART_Type * base, uart_interrupt_t intSrc);

#if FEATURE_UART_HAS_DMA_ENABLE
/*!
 * @brief Enable or disable the TX DMA request.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] enable  true = enable, false = disable.
 */
static inline void UART_SetTxDmaCmd(UART_Type * base, bool enable)
{
    uint32_t tempReg;
    SDK_ENTER_CRITICAL();
    tempReg = base->CTRL0;
    tempReg &= ~UART_CTRL0_TXDMAEN_MASK;
    tempReg |= UART_CTRL0_TXDMAEN(enable ? 1U : 0U);
    base->CTRL0 = tempReg;
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Enable or disable the RX DMA request.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] enable  true = enable, false = disable.
 */
static inline void UART_SetRxDmaCmd(UART_Type * base, bool enable)
{
    uint32_t tempReg;
    SDK_ENTER_CRITICAL();
    tempReg = base->CTRL0;
    tempReg &= ~UART_CTRL0_RXDMAEN_MASK;
    tempReg |= UART_CTRL0_RXDMAEN(enable ? 1U : 0U);
    base->CTRL0 = tempReg;
    SDK_EXIT_CRITICAL();
}
#endif

/*@}*/

/*!
 * @name Data Transfer
 * @{
 */

/*!
 * @brief Write an 8-bit character to the DATA register.
 *
 * @param[in] base  UART peripheral base pointer.
 * @param[in] data  8-bit data to transmit.
 */
static inline void UART_Putchar(UART_Type * base, uint8_t data)
{
    base->DATA = data;
}

/*!
 * @brief Write a 9-bit character to the DATA register.
 *
 * @param[in] base  UART peripheral base pointer.
 * @param[in] data  9-bit data to transmit (lower 9 bits used).
 */
void UART_Putchar9(UART_Type * base, uint16_t data);

/*!
 * @brief Write a 10-bit character to the DATA register.
 *
 * @param[in] base  UART peripheral base pointer.
 * @param[in] data  10-bit data to transmit (lower 10 bits used).
 */
void UART_Putchar10(UART_Type * base, uint16_t data);

/*!
 * @brief Read a 7-bit character from the DATA register.
 *
 * @param[in]  base      UART peripheral base pointer.
 * @param[out] readData  Pointer to store the received 7-bit data.
 */
void UART_Getchar7(const UART_Type * base, uint8_t *readData);

/*!
 * @brief Read an 8-bit character from the DATA register.
 *
 * @param[in]  base      UART peripheral base pointer.
 * @param[out] readData  Pointer to store the received 8-bit data.
 */
void UART_Getchar8(const UART_Type * base, uint8_t *readData);

/*!
 * @brief Read a 9-bit character from the DATA register.
 *
 * @param[in]  base      UART peripheral base pointer.
 * @param[out] readData  Pointer to store the received 9-bit data.
 */
void UART_Getchar9(const UART_Type * base, uint16_t *readData);

/*!
 * @brief Read a 10-bit character from the DATA register.
 *
 * @param[in]  base      UART peripheral base pointer.
 * @param[out] readData  Pointer to store the received 10-bit data.
 */
void UART_Getchar10(const UART_Type * base, uint16_t *readData);

/*@}*/

/*!
 * @name UART Status Flags
 * @{
 */

/*!
 * @brief Query a UART status flag.
 *
 * @param[in] base        UART peripheral base pointer.
 * @param[in] statusFlag  Status flag to check.
 * @return true if the flag is set.
 */
bool UART_GetStatusFlag(const UART_Type * base, uart_status_flag_t statusFlag);

/*!
 * @brief Clear a UART status flag.
 *
 * @param[in] base        UART peripheral base pointer.
 * @param[in] statusFlag  Status flag to clear.
 * @return STATUS_SUCCESS or STATUS_ERROR.
 */
status_t UART_ClearStatusFlag(UART_Type * base, uart_status_flag_t statusFlag);

/*@}*/

/*!
 * @name Error, Idle, and FIFO Management
 * @{
 */

/*!
 * @brief Enable or disable UART error interrupts (parity, noise, overrun, framing).
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] enable  true = enable, false = disable.
 */
void UART_SetErrorInterrupts(UART_Type * base, bool enable);

/*!
 * @brief Clear all driver-handled error flags in one shot.
 *
 * Clears OVR, NOZ, FE, IDLE, and PAR interrupt flags.
 *
 * @param[in] base  UART peripheral base pointer.
 */
static inline void UART_DRV_ClearErrorFlags(UART_Type * base)
{
    uint32_t mask = UART_INTF_OVRIF_MASK | \
                    UART_INTF_NOZIF_MASK | \
                    UART_INTF_FEIF_MASK  | \
                    UART_INTF_IDLEIF_MASK| \
                    UART_INTF_PARIF_MASK;

    base->INTF = (base->INTF & (~FEATURE_UART_INTF_REG_FLAGS_MASK)) | mask;
}

/*!
 * @brief Set the idle-line detection length.
 *
 * @param[in] base  UART peripheral base pointer.
 * @param[in] size  Idle-line size selector.
 */
static inline void UART_SetLineIdleSize(UART_Type * base, uint8_t size)
{
    base->CTRL1 &= ~(UART_CTRL1_IDLESZ_MASK);
    base->CTRL1 |= UART_CTRL1_IDLESZ(size);
}
#if !defined(FEATURE_UART_LITE_VERSION)
/*!
 * @brief Get the hardware TX FIFO depth (in words).
 *
 * @param[in] base  UART peripheral base pointer.
 * @return TX FIFO depth.
 */
static inline uint32_t UART_GetTxFifoSize(UART_Type * base)
{
    return ((uint32_t)0x01U << ((base->TXFIFO & UART_TXFIFO_SIZE_MASK) >> UART_TXFIFO_SIZE_SHIFT));
}

/*!
 * @brief Get the hardware RX FIFO depth (in words).
 *
 * @param[in] base  UART peripheral base pointer.
 * @return RX FIFO depth.
 */
static inline uint32_t UART_GetRxFifoSize(UART_Type * base)
{
    return ((uint32_t)0x01U << ((base->RXFIFO & UART_RXFIFO_SIZE_MASK) >> UART_RXFIFO_SIZE_SHIFT));
}

/*!
 * @brief Flush the TX FIFO.
 *
 * @param[in] base  UART peripheral base pointer.
 */
static inline void UART_ResetTxFifo(UART_Type * base)
{
    base->TXFIFO = UART_TXFIFO_RESET_MASK;
}

/*!
 * @brief Flush the RX FIFO.
 *
 * @param[in] base  UART peripheral base pointer.
 */
static inline void UART_ResetRxFifo(UART_Type * base)
{
    base->RXFIFO = UART_RXFIFO_RESET_MASK;
}

/*!
 * @brief Enable or disable the TX FIFO.
 *
 * @param[in] base      UART peripheral base pointer.
 * @param[in] isEnable  true = enable, false = disable.
 */
static inline void UART_EnableTxFifo(UART_Type * base, bool isEnable)
{
    if(isEnable)
    {
        base->TXFIFO |= UART_TXFIFO_EN(1);
    }
    else 
    {
        base->TXFIFO &= ~UART_TXFIFO_EN_MASK;
    }
}

/*!
 * @brief Enable or disable the RX FIFO.
 *
 * @param[in] base      UART peripheral base pointer.
 * @param[in] isEnable  true = enable, false = disable.
 */
static inline void UART_EnableRxFifo(UART_Type * base, bool isEnable)
{
    if(isEnable)
    {
        base->RXFIFO |= UART_RXFIFO_EN(1);
    }
    else 
    {
        base->RXFIFO &= ~UART_RXFIFO_EN_MASK;
    }
}

/*!
 * @brief Get the current number of words in the TX FIFO.
 *
 * @param[in] base  UART peripheral base pointer.
 * @return Word count in TX FIFO.
 */
static inline uint32_t UART_GetWordCountInTxFifo(UART_Type * base)
{
    return ((base->TXFIFO & UART_TXFIFO_COUNT_MASK) >> UART_TXFIFO_COUNT_SHIFT);
}

/*!
 * @brief Get the current number of words in the RX FIFO.
 *
 * @param[in] base  UART peripheral base pointer.
 * @return Word count in RX FIFO.
 */
static inline uint32_t UART_GetWordCountInRxFifo(UART_Type * base)
{
    return ((base->RXFIFO & UART_RXFIFO_COUNT_MASK) >> UART_RXFIFO_COUNT_SHIFT);
}

/*!
 * @brief Read the current TX FIFO watermark level.
 *
 * @param[in] base  UART peripheral base pointer.
 * @return TX FIFO watermark.
 */
static inline uint32_t UART_GetTxFifoWatermark(UART_Type * base)
{
    return ((base->TXFIFO & UART_TXFIFO_WATER_MASK) >> UART_TXFIFO_WATER_SHIFT);
}

/*!
 * @brief Read the current RX FIFO watermark level.
 *
 * @param[in] base  UART peripheral base pointer.
 * @return RX FIFO watermark.
 */
static inline uint32_t UART_GetRxFifoWatermark(UART_Type * base)
{
    return ((base->RXFIFO & UART_RXFIFO_WATER_MASK) >> UART_RXFIFO_WATER_SHIFT);
}

/*!
 * @brief Set the TX FIFO watermark level.
 *
 * @param[in] base            UART peripheral base pointer.
 * @param[in] watermarkValue  Watermark threshold.
 */
static inline void UART_SetTxFifoWatermark(UART_Type * base, uint32_t watermarkValue)
{
    base->TXFIFO &= ~UART_TXFIFO_WATER_MASK;
    base->TXFIFO |= UART_TXFIFO_WATER(watermarkValue);
}

/*!
 * @brief Set the RX FIFO watermark level.
 *
 * @param[in] base            UART peripheral base pointer.
 * @param[in] watermarkValue  Watermark threshold.
 */
static inline void UART_SetRxFifoWatermark(UART_Type * base, uint32_t watermarkValue)
{
    base->RXFIFO &= ~UART_RXFIFO_WATER_MASK;
    base->RXFIFO |= UART_RXFIFO_WATER(watermarkValue);
}
#endif /* !defined(FEATURE_UART_LITE_VERSION) */

/*@}*/

/*!
 * @name Modem (CTS / RTS) Control
 * @{
 */

#if FEATURE_UART_HAS_MODEM_SUPPORT
/*!
 * @brief Enable or disable CTS-based TX flow control.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] enable  true = enable, false = disable.
 */
static inline void UART_EnableTxCts(UART_Type * base, bool enable)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_TXCTSE_MASK;
    base->CTRL0 |= UART_CTRL0_TXCTSE(enable ? 1U : 0U);
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Select the CTS input source.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] source  CTS source selector.
 */
static inline void UART_SetTxCtsSource(UART_Type * base, uart_tx_cts_source_t source)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_TXCTSRC_MASK;
    base->CTRL0 |= UART_CTRL0_TXCTSRC(source);
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Set the CTS detection timing.
 *
 * @param[in] base            UART peripheral base pointer.
 * @param[in] tx_cts_det_time CTS detection time selector.
 */
static inline void UART_SetTxCtsDetectionTime(UART_Type * base, uart_tx_cts_detection_time_t tx_cts_det_time)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_TXCTSC_MASK;
    base->CTRL0 |= UART_CTRL0_TXCTSC(tx_cts_det_time);
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Enable or disable the RX RTS signal.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] enable  true = enable, false = disable.
 */
static inline void UART_EnableRxRts(UART_Type * base, bool enable)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_RXRTSE_MASK;
    base->CTRL0 |= UART_CTRL0_RXRTSE(enable ? 1U : 0U);
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Query whether RX RTS is currently enabled.
 *
 * @param[in] base  UART peripheral base pointer.
 * @return true if RX RTS is enabled.
 */
static inline bool UART_IsEnableRxRts(UART_Type * base)
{
    return (((base->CTRL0 & UART_CTRL0_RXRTSE_MASK) >> UART_CTRL0_RXRTSE_SHIFT) != 0U);
}

/*!
 * @brief Set the RX RTS FIFO watermark.
 *
 * @param[in] base       UART peripheral base pointer.
 * @param[in] watermark  RTS watermark level.
 */
static inline void UART_SetRxRtsWatermark(UART_Type * base, uart_rx_rts_watermark_t watermark)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_RTSWATER_MASK;
    base->CTRL0 |= UART_CTRL0_RTSWATER(watermark);
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Enable or disable the TX RTS signal.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] enable  true = enable, false = disable.
 */
static inline void UART_EnableTxRts(UART_Type * base, bool enable)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_TXRTSE_MASK;
    base->CTRL0 |= UART_CTRL0_TXRTSE(enable ? 1U : 0U);
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Query whether TX RTS is currently enabled.
 *
 * @param[in] base  UART peripheral base pointer.
 * @return true if TX RTS is enabled.
 */
static inline bool UART_IsEnableTxRts(UART_Type * base)
{
    return (((base->CTRL0 & UART_CTRL0_TXRTSE_MASK) >> UART_CTRL0_TXRTSE_SHIFT) != 0U);
}

/*!
 * @brief Set the TX RTS output polarity.
 *
 * @param[in] base      UART peripheral base pointer.
 * @param[in] polarity  RTS polarity selector.
 */
static inline void UART_SetTxRtsPolarity(UART_Type * base, uart_tx_rts_polarity_t polarity)
{
    SDK_ENTER_CRITICAL();
    base->CTRL0 &= ~UART_CTRL0_TXRTXPOL_MASK;
    base->CTRL0 |= UART_CTRL0_TXRTXPOL(polarity);
    SDK_EXIT_CRITICAL();
}
#endif

/*!
 * @name One-Wire Mode
 * @{
 */

#if defined(FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT) && (FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT == 1U) 
/*!
 * @brief Enable or disable UART loop mode.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] enable  true = enable, false = disable.
 */
static inline void UART_EnableLoopMode(UART_Type * base, bool enable)
{
    base->CTRL1 &= ~UART_CTRL1_LOOP_MASK;
    base->CTRL1 |= UART_CTRL1_LOOP(enable ? 1U : 0U);
}

/*!
 * @brief Enable or disable single-wire (one-wire) mode.
 *
 * @param[in] base    UART peripheral base pointer.
 * @param[in] enable  true = enable, false = disable.
 */
static inline void UART_EnableOneWireMode(UART_Type * base, bool enable)
{
    base->CTRL1 &= ~UART_CTRL1_ONEWIRE_MASK;
    base->CTRL1 |= UART_CTRL1_ONEWIRE(enable ? 1U : 0U);
}

/*!
 * @brief Set the TX line direction in one-wire mode.
 *
 * @param[in] base       UART peripheral base pointer.
 * @param[in] direction  TX line direction selector.
 */
static inline void UART_SetTxLineDirection(UART_Type * base, uart_tx_line_derection_t direction)
{
    base->CTRL1 &= ~UART_CTRL1_TXDIR_MASK;
    base->CTRL1 |= UART_CTRL1_TXDIR(direction);
}
#endif
/*@}*/

#if defined(__cplusplus)
}
#endif

#endif /* UART_HW_ACCESS_H__ */
/*******************************************************************************
 * EOF
 ******************************************************************************/

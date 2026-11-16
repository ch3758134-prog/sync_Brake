/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file status.h
 * @version 1.4.1
 * @brief Shared status-code definitions for platform/devices.
 */

#ifndef STATUS_H
#define STATUS_H

/*!
 * @addtogroup devices_status
 * @brief Common return codes shared across platform/devices and driver modules.
 * @details
 * The `status_t` enumeration reserves 0x100-sized ranges for subsystems so
 * shared drivers can report both generic and module-specific failures without
 * colliding with each other.
 *
 * | Range | Ownership |
 * |-------|-----------|
 * | `0x000 - 0x0FF` | Generic platform status codes |
 * | `0x100 - 0x1FF` | MCU services |
 * | `0x200 - 0x2FF` | I2C services |
 * | `0x300 - 0x3FF` | CAN services |
 * | `0x500 - 0x5FF` | SPI services |
 * | `0x600 - 0x6FF` | UART services |
 * | `0x700 - 0x7FF` | LINFlexD services |
 * | `0x800 - 0x8FF` | SBC services |
 * | `0x900 - 0x9FF` | EFM services |
 * | `0xA00 - 0xAFF` | SCU services |
 * | `0xB00 - 0xBFF` | HCU services |
 * | `0xC00 - 0xCFF` | SAI services |
 * | `0x1100 - 0x11FF` | FLEXCAN services |
 * | `0x1200 - 0x13FF` | ENET and PHY services |
 * | `0x1400 - 0x14FF` | eTMR services |
 * | `0x1500 - 0x15FF` | FMU services |
 * | `0x1600 - 0x16FF` | MPWM services |
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief Shared status-code enumeration.
 */
typedef enum
{
    /* Generic platform status codes. */
    STATUS_SUCCESS                         = 0x000U,    /*!< Operation completed successfully. */
    STATUS_ERROR                           = 0x001U,    /*!< Operation failed for an unspecified reason. */
    STATUS_BUSY                            = 0x002U,    /*!< Resource is busy and cannot accept the request. */
    STATUS_TIMEOUT                         = 0x003U,    /*!< Operation timed out before completion. */
    STATUS_UNSUPPORTED                     = 0x004U,    /*!< Requested feature or operation is not supported. */
    STATUS_NOCLOCK                         = 0x005U,    /*!< Required clock source is not available or not enabled. */
    STATUS_InvalidArgument                 = 0x006U,    /*!< One or more input arguments are invalid. */

    /* MCU service status codes. */
    STATUS_MCU_GATED_OFF                   = 0x100U,    /*!< The target module is currently clock-gated off. */
    STATUS_MCU_TRANSITION_FAILED           = 0x101U,    /*!< An MCU mode or state transition failed. */
    STATUS_MCU_INVALID_STATE               = 0x102U,    /*!< The requested operation is invalid in the current MCU state. */
    STATUS_MCU_NOTIFY_BEFORE_ERROR         = 0x103U,    /*!< A BEFORE notification handler reported an error. */
    STATUS_MCU_NOTIFY_AFTER_ERROR          = 0x104U,    /*!< An AFTER notification handler reported an error. */

    /* I2C service status codes. */
    STATUS_I2C_RECEIVED_NACK               = 0x200U,    /*!< The addressed target responded with NACK. */
    STATUS_I2C_TX_UNDERRUN                 = 0x201U,    /*!< The transmit path underrun while data was being sent. */
    STATUS_I2C_RX_OVERRUN                  = 0x202U,    /*!< The receive path overrun before data was serviced. */
    STATUS_I2C_ARBITRATION_LOST            = 0x203U,    /*!< The controller lost bus arbitration. */
    STATUS_I2C_ABORTED                     = 0x204U,    /*!< The active I2C transfer was aborted. */
    STATUS_I2C_BUS_BUSY                    = 0x205U,    /*!< The I2C bus is busy and cannot start a new transfer. */
    STATUS_I2C_LINE_LOW_TIMEOUT            = 0x206U,    /*!< SCL or SDA remained low until the timeout expired. */

    /* CAN service status codes. */
    STATUS_CAN_BUFF_OUT_OF_RANGE           = 0x300U,    /*!< The requested CAN buffer index is outside the valid range. */
    STATUS_CAN_NO_TRANSFER_IN_PROGRESS     = 0x301U,    /*!< No CAN transfer is currently in progress. */

    /* SPI service status codes. */
    STATUS_SPI_TX_UNDERRUN                 = 0x500U,    /*!< The SPI transmit path underrun while shifting data out. */
    STATUS_SPI_RX_OVERRUN                  = 0x501U,    /*!< The SPI receive path overrun before data was serviced. */
    STATUS_SPI_ABORTED                     = 0x502U,    /*!< The active SPI transfer was aborted. */

    /* UART service status codes. */
    STATUS_UART_TX_UNDERRUN                = 0x600U,    /*!< The UART transmit path underrun while sending data. */
    STATUS_UART_RX_OVERRUN                 = 0x601U,    /*!< The UART receive path overrun before data was serviced. */
    STATUS_UART_ABORTED                    = 0x602U,    /*!< The active UART transfer was aborted. */
    STATUS_UART_FRAMING_ERROR              = 0x603U,    /*!< A UART framing error was detected. */
    STATUS_UART_PARITY_ERROR               = 0x604U,    /*!< A UART parity error was detected. */
    STATUS_UART_NOISE_ERROR                = 0x605U,    /*!< A UART noise error was detected. */
    STATUS_UART_IDLE_ERROR                 = 0x606U,    /*!< A UART idle-line error was detected. */

    /* LINFlexD service status codes. */
    STATUS_LINFlexD_NO_HEADER_RECEIVED     = 0x700U,    /*!< No LINFlexD header was received. */
    STATUS_LINFlexD_ABORT_TIMEOUT          = 0x701U,    /*!< Timed out while aborting a LINFlexD transfer. */

    /* SBC service status codes. */
    SBC_NVN_ERROR                          = 0x801U,    /*!< Writing the non-volatile memory area failed. */
    SBC_COMM_ERROR                         = 0x802U,    /*!< Communication with the SBC was aborted or corrupted. */
    SBC_CMD_ERROR                          = 0x804U,    /*!< The SBC rejected the supplied command. */
    SBC_ERR_NA                             = 0x808U,    /*!< The requested SBC feature or device is not available. */
    SBC_MTPNV_LOCKED                       = 0x810U,    /*!< MTPNV cells are locked and cannot be written. */

    /* EFM service status codes. */
    STATUS_EFM_ERROR_ENABLE                = 0x901U,    /*!< The requested EFM operation cannot be enabled. */
    STATUS_EFM_ERROR_NO_BLOCK              = 0x902U,    /*!< No EFM blocks were enabled for the requested operation. */
    STATUS_EFM_BUSY                        = 0x903U,    /*!< The EFM module is currently busy. */
    STATUS_EFM_PROG_SIZE_ERROR             = 0x904U,    /*!< The EFM programming size is invalid. */
    STATUS_EFM_DATA_NOT_ALIGNED_ERROR      = 0x905U,    /*!< EFM programming data is not aligned as required. */
    STATUS_EFM_ADDRESS_OUT_OF_RANGE        = 0x906U,    /*!< The requested EFM address is outside the valid range. */
    STATUS_EFM_PROGRAM_EXECUTE_ERROR       = 0x907U,    /*!< An EFM program command failed while executing. */
    STATUS_EFM_NOT_SUPPORTED_CMD_ERROR     = 0x908U,    /*!< The requested EFM program command is not supported. */
    STATUS_EFM_CMD_EXECUTE_ERROR           = 0x909U,    /*!< An EFM command failed while executing. */

    STATUS_EdPROM_ID_OUT_OF_RANGE          = 0x980U,    /*!< The Emulated EEPROM identifier is outside the valid range. */
    STATUS_EdPROM_ID_NOT_FOUND             = 0x981U,    /*!< The requested Emulated EEPROM identifier was not found. */
    STATUS_EdPROM_MAKE_DEAD_ERROR          = 0x982U,    /*!< Marking an Emulated EEPROM sector as dead failed. */
    STATUS_EdPROM_MAKE_DEAD_SUCCESS        = 0x983U,    /*!< Marking an Emulated EEPROM sector as dead succeeded. */
    STATUS_EdPROM_SECTOR_ERASE_ERROR       = 0x984U,    /*!< Erasing an Emulated EEPROM sector failed. */
    STATUS_EdPROM_PROG_VERIFY_ERROR        = 0x985U,    /*!< Emulated EEPROM programming verification failed. */

    /* SCU service status codes. */
    STATUS_SCU_CLK_SWITCH_TIMEOUT          = 0xA01U,    /*!< Timed out while switching the system clock source. */
    STATUS_SCU_DIVIDER_SWITCH_TIMEOUT      = 0xA02U,    /*!< Timed out while switching the system clock divider. */
    STATUS_SCU_PLL_LOCK_TIMEOUT            = 0xA03U,    /*!< Timed out while waiting for PLL lock. */
    STATUS_SCU_FIRC_READY_TIMEOUT          = 0xA04U,    /*!< Timed out while waiting for FIRC ready. */
    STATUS_SCU_SIRC_READY_TIMEOUT          = 0xA05U,    /*!< Timed out while waiting for SIRC ready. */
    STATUS_SCU_FXOSC_READY_TIMEOUT         = 0xA06U,    /*!< Timed out while waiting for FXOSC ready. */
    STATUS_SCU_SXOSC_READY_TIMEOUT         = 0xA07U,    /*!< Timed out while waiting for SXOSC ready. */

    /* HCU service status codes. */
    STATUS_HCU_KEY_SIZE_NOT_SUPPORTED      = 0xB01U,    /*!< The requested HCU key size is not supported. */
    STATUS_HCU_LOAD_KEY_WHEN_BUSY          = 0xB02U,    /*!< A key load was requested while the HCU was busy. */
    STATUS_HCU_CCM_TAG_SIZE_ERROR          = 0xB04U,    /*!< The configured CCM tag size is invalid. */
    STATUS_HCU_CCM_NONCE_DATA_SIZE_ERROR   = 0xB08U,    /*!< The configured CCM nonce size is invalid. */
    STATUS_HCU_CCM_NOT_CONFIGURED_ERROR    = 0xB0CU,    /*!< The HCU CCM context is incomplete or not configured. */
    STATUS_HCU_MAC_CHECK_ERROR             = 0xB10U,    /*!< MAC verification failed. */
    STATUS_HCU_HASH_CHECK_ERROR            = 0xB20U,    /*!< Hash verification failed. */
    STATUS_HCU_ECDSA_CHECK_ERROR           = 0xB30U,    /*!< ECDSA signature verification failed. */
    STATUS_HCU_ECDSA_NO_INPUT_ERROR        = 0xB40U,    /*!< Required ECDSA input data was missing. */
    STATUS_HCU_ECDSA_SIG_OVER_ERROR        = 0xB50U,    /*!< The ECDSA signature exceeded the valid curve range. */

    /* SAI service status codes. */
    STATUS_SAI_ABORTED                     = 0xC01U,    /*!< The active SAI transfer was aborted. */

    /* FLEXCAN service status codes. */
    STATUS_FLEXCAN_BUFF_OUT_OF_RANGE       = 0x1100U,   /*!< The requested FLEXCAN buffer index is outside the valid range. */
    STATUS_FLEXCAN_NO_TRANSFER_IN_PROGRESS = 0x1101U,   /*!< No FLEXCAN transfer is currently in progress. */

    /* ENET and PHY service status codes. */
    STATUS_ENET_InitMemoryFail             = 0x1200U,   /*!< ENET QoS initialization failed because memory allocation failed. */
    STATUS_ENET_RxFrameError               = 0x1201U,   /*!< An ENET frame was received with data corruption. */
    STATUS_ENET_RxFrameFail                = 0x1202U,   /*!< Receiving an ENET frame failed. */
    STATUS_ENET_RxFrameEmpty               = 0x1203U,   /*!< No ENET frame is currently available. */
    STATUS_ENET_RxFrameDrop                = 0x1204U,   /*!< An ENET frame was dropped because no receive buffer was available. */
    STATUS_ENET_TxFrameBusy                = 0x1205U,   /*!< ENET transmit descriptors are still in use. */
    STATUS_ENET_TxFrameFail                = 0x1206U,   /*!< Transmitting an ENET frame failed. */
    STATUS_ENET_TxFrameOverLen             = 0x1207U,   /*!< The ENET transmit frame is longer than the supported limit. */
    STATUS_ENET_Est_SwListBusy             = 0x1208U,   /*!< The EST software GCL list is still being processed. */
    STATUS_ENET_Est_SwListWriteAbort       = 0x1209U,   /*!< Writing the EST software GCL list was aborted. */
    STATUS_ENET_Est_InvalidParameter       = 0x1210U,   /*!< An EST GCL list parameter is invalid. */
    STATUS_ENET_Est_BtrError               = 0x1211U,   /*!< Loading the EST list failed because of a base-time error. */
    STATUS_ENET_TrgtBusy                   = 0x1212U,   /*!< The ENET target-time register is busy. */
    STATUS_ENET_Timeout                    = 0x1213U,   /*!< An ENET operation timed out. */
    STATUS_ENET_PpsBusy                    = 0x1214U,   /*!< The ENET PPS command interface is busy. */
    STATUS_PHY_SMIVisitTimeout             = 0x1300U,   /*!< Timed out while accessing the PHY through the SMI interface. */

    /* eTMR service status codes. */
    STATUS_ETMR_CAPTURE_NOT_READY          = 0x1400U,   /*!< eTMR capture data is not ready yet. */
    STATUS_ETMR_LOAD_FREQ_ERROR            = 0x1401U,   /*!< The requested eTMR load frequency is invalid. */
    STATUS_ETMR_TRIG_OUTPUT_FREQ_ERROR     = 0x1402U,   /*!< The requested eTMR trigger-output frequency is invalid. */

    /* FMU service status codes. */
    STATUS_FMU_ERROR_CONFIG_TIMEOUT        = 0x1500U,   /*!< Timed out while entering FMU configuration state. */
    STATUS_FMU_ERROR_INIT_FMU              = 0x1501U,   /*!< FMU module initialization failed. */
    STATUS_FMU_ERROR_SET_CFG               = 0x1502U,   /*!< Entering FMU configuration mode failed. */
    STATUS_FMU_ERROR_SET_RUN               = 0x1503U,   /*!< Entering FMU run mode failed. */
    STATUS_FMU_ERROR_APPLY_CH_CONFIG       = 0x1504U,   /*!< Applying FMU channel-fault configuration failed. */
    STATUS_FMU_ERROR_SET_EOUT              = 0x1505U,   /*!< Applying FMU EOUT configuration failed. */
    STATUS_FMU_ERROR_FAULT_DETECTED        = 0x1506U,   /*!< The FMU reported one or more active faults. */
    STATUS_FMU_ERROR_OP_ABORT              = 0x1507U,   /*!< The FMU aborted the requested operation. */
    STATUS_FMU_ERROR_OTHER                 = 0x1508U,   /*!< An unspecified FMU error occurred. */

    /* MPWM service status codes. */
    STATUS_MPWM_ERROR_FUNC_CLK_FREQ        = 0x1600U,   /*!< MPWM functional clock frequency exceeds the supported limit. */
} status_t;

/*! @} */ /* End of devices_status */

#endif /* STATUS_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/

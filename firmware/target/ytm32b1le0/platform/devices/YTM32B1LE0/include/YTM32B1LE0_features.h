/*
 * Copyright (c) 2021-2025 Yuntu Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*!
 * @file YTM32B1LE0_features.h
 */

#ifndef YTM32B1LE0_FEATURES_H
#define YTM32B1LE0_FEATURES_H

/* ERRATA sections*/

/* @brief Number of cores. */
#define NUMBER_OF_CORES                     (1u)

/* SOC module features */

/* @brief PORT availability on the SoC. */
#define FEATURE_SOC_PORT_COUNT                (5)

#define FEATURE_SOC_IPC_COUNT                 (1)

#define FEATURE_SOC_HAS_SEPARATE_CMU_AND_CLKOUT (0)

#define FEATURE_SCU_SUPPORT_PLL            (0)
#define FEATURE_SCU_SUPPORT_CMU_REFCLK_SEL (0)
/* @brief SIRC clock frequency(2MHz). */
#define FEATURE_SCU_SIRC_FREQ                 (2000000U)
/* @brief Fast IRC clock frequency(48MHz). */
#define FEATURE_SCU_FIRC_FREQ                 (48000000U)
/* @brief LPO clock frequency(1KHz). */
#define FEATURE_SCU_LPO_FREQ                  (1000U)
/* @brief FXOSC clock frequency(24MHz). */
#define FEATURE_SCU_FXOSC_FREQ                (24000000U)
#define FEATURE_SCU_SUPPORT_SXOSC             (1)
#if FEATURE_SCU_SUPPORT_SXOSC
/* @brief SXOSC clock frequency(32.768K). */
#define FEATURE_SCU_SXOSC_FREQ                (32768U)
#endif /* FEATURE_SCU_SUPPORT_SXOSC */
/* @brief Clock switch PLL lock timeout loop count. */
#define FEATURE_CLOCK_TIMEOUT_LOOP_CNT        (0x3FFFFU)
/* @brief VECTKEY value so that AIRCR register write is not ignored. */
#define FEATURE_SCB_VECTKEY                   (0x05FAU)

/* @brief RCU support high voltage detect */
#define FEATURE_RCU_HVD_RESET                 (0U)
/* @brief RCU support low voltage detect */
#define FEATURE_RCU_LVD_RESET                 (1U)
/* @brief RCU support debug detect */
#define FEATURE_RCU_DBG_RESET                 (0U)

/* FLASH module features */

/* @brief Flash operation clock source, used for PRESCALER */
#define FEATURE_EFM_OPERATION_CLOCK_SOURCE   (SLOW_BUS_CLK)
/* @brief Flash block count. */
#define FEATURE_EFM_BLOCK_COUNT              (1u)
/* @brief Flash program flash start address. */
#define FEATURE_EFM_MAIN_ARRAY_START_ADDRESS (0x0U)
/* @brief Flash program flash end address. */
#define FEATURE_EFM_MAIN_ARRAY_END_ADDRESS   (0x1FFFFU)
/* @brief Flash sector size. */
#define FEATURE_EFM_MAIN_ARRAY_SECTOR_SIZE   (0x200u)
/* @brief Flash has data flash. */
#define FEATURE_EFM_HAS_DATA_FLASH           (1)
#if FEATURE_EFM_HAS_DATA_FLASH
/* @brief Flash data flash start address. */
#define FEATURE_EFM_DATA_ARRAY_START_ADDRESS (0x10000000U)
/* @brief Flash data flash end address. */
#define FEATURE_EFM_DATA_ARRAY_END_ADDRESS   (0x100007FFU)
/* @brief Flash data flash sector size. */
#define FEATURE_EFM_DATA_ARRAY_SECTOR_SIZE   (0x200u)
#endif /* FEATURE_EFM_HAS_DATA_FLASH */
#define FEATURE_EFM_HAS_NVR_FLASH            (1)
#if FEATURE_EFM_HAS_NVR_FLASH
/* @brief Flash NVR start address. */
#define FEATURE_EFM_NVR_ARRAY_START_ADDRESS  (0x10000800U)
/* @brief Flash NVR end address. */
#define FEATURE_EFM_NVR_ARRAY_END_ADDRESS    (0x10000DFFU)
/* @brief Flash NVR sector size. */
#define FEATURE_EFM_NVR_ARRAY_SECTOR_SIZE    (0x200u)
#endif /* FEATURE_EFM_HAS_NVR_FLASH */

/* @brief Flash min sector size. */
#define FEATURE_EFM_FLASH_MIN_SECTOR_SIZE   (0x200u)

/* @brief Flash write unit size. */
#define FEATURE_EFM_WRITE_UNIT_SIZE         (4u)
/* @brief Flash write unit size in words. */
#define FEATURE_EFM_WRITE_UNIT_WORD_SIZE    (FEATURE_EFM_WRITE_UNIT_SIZE >> 2)
/* @brief Erase Flash Block command. */
#define FEATURE_EFM_ERASE_BLOCK_CMD_CODE    (0x12u)
/* @brief Erase Flash Sector command. */
#define FEATURE_EFM_ERASE_SECTOR_CMD_CODE   (0x10u)
/* @brief Program Flash Sector command. */
#define FEATURE_EFM_PROGRAM_CMD_CODE        (0x02u)
/* @brief Flash Status Code to Clear Mask before write command. */
#define FEATURE_EFM_CMD_STS_CLEAR_MASK      (EFM_STS_DONE_MASK | EFM_STS_ACCERR_MASK | EFM_STS_UNRECOVERR_MASK | EFM_STS_RECOVERR_MASK)
/* @brief Flash Status Error Code Mask. */
#define FEATURE_EFM_CMD_ERROR_MASK          (EFM_STS_ACCERR_MASK | EFM_STS_UNRECOVERR_MASK)
/* @brief Flash has read-only protection bit feature */
#define FEATURE_EFM_HAS_READONLY_BIT        (1U)
/* @brief Flash software read verify */
#define FEATURE_EFM_SW_READ_VERIFY          (1U)
/* @brief Flash data preread check before programming*/
#define FEATURE_EFM_PREREAD_CHECK_FOR_PROGRAM      (1U)

/* @brief Flash Unlock command register. */
#define EFM_UNLOCK_CMD_REGISTER(base)                   \
    do {                                                \
        (base)->CMD_UNLOCK = 0xfd9573f5U;               \
    } while (0)
/* @brief Flash enable flash write commands. */
#define EFM_ENABLE_WE_COMMAND(base)                     \
    do                                                  \
    {                                                   \
    } while (0)
/* @brief Flash disable flash write commands. */
#define EFM_DISABLE_WE_COMMAND(base)                    \
    do                                                  \
    {                                                   \
    } while (0)

/* RCM module feature */

/* @brief Has existence of CMU loss of clock as reset source */
#define FEATURE_RCM_HAS_EXISTENCE_CMU_LOSS_OF_CLOCK     (1)
/* @brief Has CMU loss of clock as reset source */
#define FEATURE_RCM_HAS_CMU_LOSS_OF_CLOCK               (1)
/* @brief Has sticky CMU loss of clock as reset source */
#define FEATURE_RCM_HAS_STICKY_CMU_LOSS_OF_CLOCK        (1)

/* WDG module features */

/*!
 * @brief WDG clock source.
 * Implements : wdg_clock_source_t_Class
 */
typedef enum
{
    WDG_SXOSC_CLOCK = 0x00U, /*!< SXOSC clock */
    WDG_SIRC_CLOCK  = 0x01U, /*!< SIRC clock */
} wdg_clock_source_t;
/* @brief The 32-bit value used for unlocking the WDG. */
#define FEATURE_WDG_UNLOCK_VALUE_1                      (0xB631)
#define FEATURE_WDG_UNLOCK_VALUE_2                      (0xC278)
/* @brief The 32-bit value used for resetting the WDG counter. */
#define FEATURE_WDG_TRIGGER_VALUE_1                     (0xA518)
#define FEATURE_WDG_TRIGGER_VALUE_2                     (0xD826)
/* @brief The reset value of the timeout register. */
#define FEATURE_WDG_TOVR_RESET_VALUE                    (0x300U)
/* @brief The value minimum of the timeout register. */
#define FEATURE_WDG_MINIMUM_TIMEOUT_VALUE               (0x0U)
/* @brief The reset value of the window register. */
#define FEATURE_WDG_WVR_RESET_VALUE                     (0x0U)
/* @brief The mask of the reserved bit in the CS register. */
#define FEATURE_WDG_CS_RESERVED_MASK                    (0x2000U)
/* @brief The value used to set WDG source clock from LPO. */
#define FEATURE_WDG_CLK_FROM_LPO                        (0x1UL)

/* @brief Default reset value of the CS register. */
#define FEATURE_WDG_CR_RESET_VALUE                      (0x42U)

/* Interrupt module features */

/* @brief Lowest interrupt request number. */
#define FEATURE_INTERRUPT_IRQ_MIN                       (NMI_IRQn)
/* @brief Highest interrupt request number. */
#define FEATURE_INTERRUPT_IRQ_MAX                       (UART0_IRQn)
/* @brief FlexCAN Embedded RAM address offset */
#define FEATURE_CAN_RAM_OFFSET                          (0x00000080u)
/**< Number of priority bits implemented in the NVIC */
#define FEATURE_NVIC_PRIO_BITS                          (2U)
/* @brief Has software interrupt. */
#define FEATURE_INTERRUPT_HAS_SOFTWARE_IRQ              (0u)
/* @brief Has pending interrupt state. */
#define FEATURE_INTERRUPT_HAS_PENDING_STATE             (1u)
/* @brief Has active interrupt state. */
#define FEATURE_INTERRUPT_HAS_ACTIVE_STATE              (0u)
/* @brief Multicore support for interrupts */
#define FEATURE_INTERRUPT_MULTICORE_SUPPORT             (0u)
/* @brief Registers in which the start of interrupt vector table needs to be configured */
#define FEATURE_INTERRUPT_INT_VECTORS                   {&SCB->VTOR}

/* eTMR module features */

#define FEATURE_eTMR_ERRATA_E503005                     (1U)
/* @brief Number of PWM channels */
#define FEATURE_eTMR_CHANNEL_COUNTS                     {8U, 8U, 2U}
#define FEATURE_eTMR_CHANNEL_INTERRUPT_COUNTS           (8U)
#define FEATURE_eTMR_CHANNEL_MAX_COUNT                  (8U)
#define FEATURE_eTMR0_CHANNEL_COUNT                     (8U)
#define FEATURE_eTMR1_CHANNEL_COUNT                     (8U)
#define FEATURE_eTMR2_CHANNEL_COUNT                     (2U)
#define FEATURE_eTMR_SUPPORT_DOUBLE_SWITCH              (0U)
#define FEATURE_eTMR_SUPPORT_DITHER                     (0U)
#define FEATURE_eTMR_SUPPORT_INIT_CONFIG                (0U)
#define FEATURE_eTMR_SUPPORT_TRIG_WIDTH_CFG             (0U)
#define FEATURE_eTMR_SUPPORT_MID_CONFIG                 (0U)
#define FEATURE_eTMR_SUPPORT_QD_MOD_CONFIG              (0U)
#define FEATURE_eTMR_SUPPORT_PULSE_MEASURE              (0U)
#define FEATURE_eTMR_DITHER_CONFIGURATION               (0U)
#define FEATURE_eTMR_SUPPORT_CHANEL_DEADTIME            (0U)
#define FEATURE_eTMR_HAS_DEAD_TIME_PRESCALER            (1U)
/* @brief Double switch */
#define FEATURE_eTMR_HAS_DOUBLE_SWITCH                  (0U)
/* @brief Combination capture mode */
#define FEATURE_eTMR_HAS_COMBINATION_CAPTURE            (0U)
/* @brief Hardware capture method */
#define FEATURE_eTMR_HAS_HARDWARE_CAPTURE               (0U)
/* @brief Capture hold */
#define FEATURE_eTMR_HAS_CAPTURE_HOLD                   (0U)
/* @brief Output trigger width */
#define FEATURE_eTMR_HAS_OUTPUT_TRIGGER_WIDTH           (0U)
/* @brief MID feature */
#define FEATURE_eTMR_HAS_MID                            (0U)
/* @brief configure counter initial value source */
#define FEATURE_eTMR_HAS_CNT_INIT_SRC                   (0U)
/* @brief Counter initial value configurable */
#define FEATURE_eTMR_HAS_INIT_REG                       (0U)
/* @brief CHINIT bit is in Channel CTRL register */
#define FEATURE_eTMR_HAS_CHINIT_BIT                     (1U)
/* @brief Quadrature decoder mod register */
#define FEATURE_eTMR_HAS_QUAD_DECODER_MOD               (0U)
/* @brief Support configure dead time alone */
#define FEATURE_eTMR_DEADTIME_CONFIG_EACH_CHANNEL       (0U)
/* @brief Counter count from 1 */
#define FEATURE_eTMR_COUNT_FROM_INIT_PLUS_ONE           (0U)
/* @brief Number of fault channels */
#define FEATURE_eTMR_FAULT_CHANNELS                     (4U)
#define eTMR_INTERNAL_CLOCK                             (FAST_BUS_CLK)
/* @brief eTMR enternal clock source frequency */
#define FEATURE_eTMR_EXTERNAL_CLOCK_FREQ                (1000000U)
/* @brief eTMR external ipc clock source */
#define eTMR_IPC_CLK                                    {eTMR0_CLK, eTMR1_CLK, eTMR2_CLK}
/* @brief Width of control channel */
#define eTMR_FEATURE_COMBMD_CHAN_CTRL_WIDTH             (8U)
/* @brief Output channel offset */
#define eTMR_FEATURE_OUTPUT_CHANNEL_OFFSET              (16U)
/* @brief Max counter value */
#define eTMR_FEATURE_CNT_MAX_VALUE_U32                  (0x0000FFFFU)
/* @brief Input capture for single shot */
#define eTMR_FEATURE_INPUT_CAPTURE_SINGLE_SHOT          (2U)
/* @brief Number of interrupt vector for channels of the eTMR module. */
#define FEATURE_eTMR_HAS_NUM_IRQS_CHANS                 (1U)

/* I2C module features */

/* @brief DMA requests for I2C module. */
#define I2C_DMA_REQ                                     {{(uint8_t)DMA_REQ_I2C0_TX, (uint8_t)DMA_REQ_I2C0_RX}, {(uint8_t)DMA_REQ_I2C1_TX, (uint8_t)DMA_REQ_I2C1_RX}}
/* @brief IPC clocks for I2C module. */
#define I2C_CLOCK_NAMES                                 {I2C0_CLK, I2C1_CLK}
/* @brief IPC index for I2C module */
#define I2C_IPC_INDEX                                   {IPC_I2C0_INDEX, IPC_I2C1_INDEX}
/* @brief I2C state initialization */
#define I2C_STATE_PTR                                   {NULL, NULL}
/* @brief Slave mode support I2C module. */
#define I2C_SLAVE_SUPPORT                               {true, false}
/* @brief I2C module support TXCFG. */
#define I2C_SUPPORT_TXCFG                               (1)
/* @brief Disable high-speed and ultra-fast operating modes */
#define I2C_HAS_FAST_PLUS_MODE                          (1U)
#define I2C_HAS_HIGH_SPEED_MODE                         (1U)
#define I2C_HAS_ULTRA_FAST_MODE                         (1U)

/* CRC module features */

/* @brief CRC module */
/* @brief CRC support CRC-4 */
#define FEATURE_CRC_SUPPORT_CRC4                        (1U)
/* @brief CRC support CRC 8 mode*/
#define FEATURE_CRC_SUPPORT_CRC8                        (0U)
/* @brief Default CRC bit width */
#define FEATURE_CRC_DEFAULT_WIDTH                       CRC_BITS_16
/* @brief Default CRC read transpose */
#define FEATURE_CRC_DEFAULT_READ_TRANSPOSE              CRC_TRANSPOSE_NONE
/* @brief Default CRC write transpose */
#define FEATURE_CRC_DEFAULT_WRITE_TRANSPOSE             CRC_TRANSPOSE_NONE
/* @brief Default polynomial 0x1021U */
#define FEATURE_CRC_DEFAULT_POLYNOMIAL                  (0x1021U)
/* @brief Default seed value is 0xFFFFU */
#define FEATURE_CRC_DEFAULT_SEED                        (0xFFFFU)

/* PCTRL module features */
/* @brief Has control lock. */
#define FEATURE_PCTRL_HAS_PIN_CONTROL_LOCK              (0)
/* @brief Has open drain control. */
#define FEATURE_PINS_HAS_OPEN_DRAIN                     (0)
/* @brief Has digital filter. */
#define FEATURE_PINS_HAS_DIGITAL_FILTER                 (1)
/* @brief Has digital filter clock selection. */
#define FEATURE_PINS_HAS_FILTER_CLOCK_SEL               (1)
/* @brief Has trigger output to trigger other peripherals. */
#define FEATURE_PINS_HAS_TRIGGER_OUT                    (1)
/* @brief Has setting flag only. */
#define FEATURE_PINS_HAS_FLAG_SET_ONLY                  (0)
/* @brief Has pull resistor selection available. */
#define FEATURE_PINS_HAS_PULL_SELECTION                 (1)
/* @brief Has slew rate control. */
#define FEATURE_PINS_HAS_SLEW_RATE                      (1)
/* @brief Has passive filter. */
#define FEATURE_PINS_HAS_PASSIVE_FILTER                 (1)
/* @brief Has drive strength. */
#define FEATURE_PINS_HAS_DRIVE_STRENGTH                 (1)
/* @brief Has drive strength control bits*/
#define FEATURE_PINS_HAS_DRIVE_STRENGTH_CONTROL         (1)
/* @brief Has port input disable control bits*/
#define FEATURE_PINS_HAS_INPUT_DISABLE                  (1)

/* ACMP module features */

/* @brief Comparator has internal DAC module */
#define FEATURE_ACMP_HAS_DAC                            (1U)
/* @brief Comparator has power mode control */
#define FEATURE_ACMP_HAS_POWER_MODE                     (1U)
/* @brief Comparator has pin output enable */
#define FEATURE_ACMP_HAS_PINOUT_ENABLE                  (1U)
/* @brief Comparator has mode config */
#define FEATURE_ACMP_HAS_EN_REG                         (1U)
/* @brief Continuous mode is in CTRL MODE bit field */
#define FEATURE_ACMP_HAS_CONTINUOUS_MODE_IN_CTRL_REG    (1U)
/* @brief ACMP Function Clock Selection */
#define FEATURE_ACMP_HAS_CLK_SRC_SEL                    (0U)
/* @brief ACMP has filter bypass control */
#define FEATURE_ACMP_HAS_FILTER_BYPASS                  (1U)
/* @brief ACMP filter clock source is in the CTRL register */
#define FEATURE_ACMP_HAS_FILTER_CLK_SRC_IN_CTRL_REG     (1U)
/* @brief ACMP positive/negative input source selected in MUX register */
#define FEATURE_ACMP_HAS_INPUT_SRC_SEL_IN_MUX_REG       (1U)
/* @brief ACMP DAC reference voltage selection */
#define FEATURE_ACMP_HAS_DAC_VOLTAGE_REF_SRC            (0U)
/* @brief ACMP DAC output enable */
#define FEATURE_ACMP_HAS_DAC_OUTPUT                     (0U)
/* @brief ACMP Auto disable hard block */
#define FEATURE_ACMP_HAS_AUTODIS                        (0U)
/* @brief ACMP comparator offset control */
#define FEATURE_ACMP_HAS_OFFSET_CONTROL                 (0U)
/* @brief ACMP support sxosc clock */
#define FEATURE_ACMP_SUPPORT_SXOSC_CLK                  (0U)
/* @brief ACMP has EXP register */
#define FEATURE_ACMP_HAS_EXP_REG                        (0U)
/* @brief ACMP has trigger mode gate */
#define FEATURE_ACMP_HAS_TRIG_MODE_GATE                 (1U)

#define FEATURE_ACMP_CTRL_RESET_VALUE                   (0x00000000)
#define FEATURE_ACMP_STS_CLEAR_MASK                     (0x00FF0000)
#define FEATURE_ACMP_STS_OUT_MASK                       (0xFF000000)
#define FEATURE_ACMP_STS_OUT_SHIFT                      (24)
#define FEATURE_ACMP_STS_INPUT_MASK                     (0x00FF0000)
#define FEATURE_ACMP_STS_INPUT_SHIFT                    (16)
#define FEATURE_ACMP_DEFAULT_CONT_PERIOD                (0x30)
#define FEATURE_ACMP_DEFAULT_CONT_POS                   (0x20)
#define FEATURE_ACMP_STS_CH_OUT_MASK                    (0xFF000000)
#define FEATURE_ACMP_STS_CH_OUT_SHIFT                   (24)
#define FEATURE_ACMP_STS_CH_FLAG_MASK                   (0x00FF0000)
#define FEATURE_ACMP_STS_CH_FLAG_SHIFT                  (16)

#if FEATURE_ACMP_HAS_DAC
#define ACMP_DAC_SOURCE                                 (0U)
#define ACMP_MUX_SOURCE                                 (1U)
#define ACMP_DAC_RESOLUTION                             (255U)
#endif /* FEATURE_ACMP_HAS_DAC */

/*! @brief Clock names. */
typedef enum {
    /* IPC clocks */
    DMA_CLK                  =  0U, /*!< DMA        clock source */
    DIVSQRT_CLK              =  1U, /*!< DIVSQRT    clock source */
    GPIO_CLK                 =  2U, /*!< GPIO       clock source */
    EMU0_CLK                 =  3U, /*!< EMU0       clock source */
    EFM_CLK                  =  4U, /*!< EFM        clock source */
    FlexCAN0_CLK             =  5U, /*!< FlexCAN0   clock source */
    SPI0_CLK                 =  6U, /*!< SPI0       clock source */
    SPI1_CLK                 =  7U, /*!< SPI1       clock source */
    SPI2_CLK                 =  8U, /*!< SPI2       clock source */
    CRC_CLK                  =  9U, /*!< CRC        clock source */
    pTMR0_CLK                = 10U, /*!< pTMR0      clock source */
    eTMR0_CLK                = 11U, /*!< eTMR0      clock source */
    eTMR1_CLK                = 12U, /*!< eTMR1      clock source */
    eTMR2_CLK                = 13U, /*!< eTMR2      clock source */
    ADC0_CLK                 = 14U, /*!< ADC0       clock source */
    RTC_CLK                  = 15U, /*!< RTC        clock source */
    lpTMR0_CLK               = 16U, /*!< lpTMR0     clock source */
    CIM_CLK                  = 17U, /*!< CIM        clock source */
    PCTRLA_CLK               = 18U, /*!< PCTRLA     clock source */
    PCTRLB_CLK               = 19U, /*!< PCTRLB     clock source */
    PCTRLC_CLK               = 20U, /*!< PCTRLC     clock source */
    PCTRLD_CLK               = 21U, /*!< PCTRLD     clock source */
    PCTRLE_CLK               = 22U, /*!< PCTRLE     clock source */
    WDG0_CLK                 = 23U, /*!< WDG0       clock source */
    TMU_CLK                  = 24U, /*!< TMU        clock source */
    SCU_CLK                  = 25U, /*!< SCU        clock source */
    I2C0_CLK                 = 26U, /*!< I2C0       clock source */
    I2C1_CLK                 = 27U, /*!< I2C1       clock source */
    UART0_CLK                = 28U, /*!< UART0      clock source */
    UART1_CLK                = 29U, /*!< UART1      clock source */
    UART2_CLK                = 30U, /*!< UART2      clock source */
    ACMP0_CLK                = 31U, /*!< ACMP0      clock source */
    STU_CLK                  = 32U, /*!< STU        clock source */
    PCU_CLK                  = 33U, /*!< PCU        clock source */
    RCU_CLK                  = 34U, /*!< RCU        clock source */

    IPC_PERI_END_OF_CLOCK    = 35U, /*!< IPC_PERI_END_OF_CLOCK */
    IPC_SIRC_CLK             = 36U, /*!< IPC_SIRC   clock source */
    IPC_FIRC_CLK             = 37U, /*!< IPC_FIRC   clock source */
    IPC_FXOSC_CLK            = 38U, /*!< IPC_FXOSC  clock source */
    IPC_SXOSC_CLK            = 39U, /*!< IPC_SXOSC  clock source */
    IPC_LPO_CLK              = 40U, /*!< IPC_LPO    clock source */

    IPC_END_OF_CLOCK         = 41U, /*!< IPC_END_OF_CLOCK */
    CORE_CLK                 = 42U, /*!< CORE       clock source */
    FAST_BUS_CLK             = 43U, /*!< FAST_BUS   clock source */
    SLOW_BUS_CLK             = 44U, /*!< SLOW_BUS   clock source */

    CLOCK_NAME_COUNT         = 45U, /*!< CLOCK_NAME_COUNT */
} clock_names_t;
/*! @brief IPC clock name mappings
 *  Mappings between clock names and peripheral clock control indexes.
 *  If there is no peripheral clock control index for a clock name,
 *  then the corresponding value is IPC_INVALID_INDEX.
 */
#define IPC_DMA_INDEX          (  8U)
#define IPC_DIVSQRT_INDEX      ( 14U)
#define IPC_GPIO_INDEX         ( 15U)
#define IPC_EMU0_INDEX         ( 24U)
#define IPC_EFM_INDEX          ( 32U)
#define IPC_FlexCAN0_INDEX     ( 36U)
#define IPC_SPI0_INDEX         ( 44U)
#define IPC_SPI1_INDEX         ( 45U)
#define IPC_SPI2_INDEX         ( 46U)
#define IPC_CRC_INDEX          ( 50U)
#define IPC_pTMR0_INDEX        ( 55U)
#define IPC_eTMR0_INDEX        ( 56U)
#define IPC_eTMR1_INDEX        ( 57U)
#define IPC_eTMR2_INDEX        ( 58U)
#define IPC_ADC0_INDEX         ( 59U)
#define IPC_RTC_INDEX          ( 61U)
#define IPC_lpTMR0_INDEX       ( 64U)
#define IPC_CIM_INDEX          ( 72U)
#define IPC_PCTRLA_INDEX       ( 73U)
#define IPC_PCTRLB_INDEX       ( 74U)
#define IPC_PCTRLC_INDEX       ( 75U)
#define IPC_PCTRLD_INDEX       ( 76U)
#define IPC_PCTRLE_INDEX       ( 77U)
#define IPC_WDG0_INDEX         ( 82U)
#define IPC_TMU_INDEX          ( 99U)
#define IPC_SCU_INDEX          (100U)
#define IPC_I2C0_INDEX         (102U)
#define IPC_I2C1_INDEX         (103U)
#define IPC_UART0_INDEX        (106U)
#define IPC_UART1_INDEX        (107U)
#define IPC_UART2_INDEX        (108U)
#define IPC_ACMP0_INDEX        (115U)
#define IPC_STU_INDEX          (119U)
#define IPC_PCU_INDEX          (125U)
#define IPC_RCU_INDEX          (127U)
#define IPC_PERI_END_OF_CLOCK_INDEX   (128U)
#define IPC_SIRC_INDEX         (129U)
#define IPC_FIRC_INDEX         (130U)
#define IPC_FXOSC_INDEX        (131U)
#define IPC_SXOSC_INDEX        (132U)
#define IPC_LPO_INDEX          (133U)
#define IPC_END_OF_CLOCK_INDEX   (134U)
#define IPC_CORE_INDEX         (135U)
#define IPC_FAST_BUS_INDEX     (136U)
#define IPC_SLOW_BUS_INDEX     (137U)

#define  IPC_CLOCK_NAME_MAPPINGS \
{                                \
    IPC_DMA_INDEX,            /*!< DMA        clock source    8u */ \
    IPC_DIVSQRT_INDEX,        /*!< DIVSQRT    clock source   14u */ \
    IPC_GPIO_INDEX,           /*!< GPIO       clock source   15u */ \
    IPC_EMU0_INDEX,           /*!< EMU0       clock source   24u */ \
    IPC_EFM_INDEX,            /*!< EFM        clock source   32u */ \
    IPC_FlexCAN0_INDEX,       /*!< FlexCAN0   clock source   36u */ \
    IPC_SPI0_INDEX,           /*!< SPI0       clock source   44u */ \
    IPC_SPI1_INDEX,           /*!< SPI1       clock source   45u */ \
    IPC_SPI2_INDEX,           /*!< SPI2       clock source   46u */ \
    IPC_CRC_INDEX,            /*!< CRC        clock source   50u */ \
    IPC_pTMR0_INDEX,          /*!< pTMR0      clock source   55u */ \
    IPC_eTMR0_INDEX,          /*!< eTMR0      clock source   56u */ \
    IPC_eTMR1_INDEX,          /*!< eTMR1      clock source   57u */ \
    IPC_eTMR2_INDEX,          /*!< eTMR2      clock source   58u */ \
    IPC_ADC0_INDEX,           /*!< ADC0       clock source   59u */ \
    IPC_RTC_INDEX,            /*!< RTC        clock source   61u */ \
    IPC_lpTMR0_INDEX,         /*!< lpTMR0     clock source   64u */ \
    IPC_CIM_INDEX,            /*!< CIM        clock source   72u */ \
    IPC_PCTRLA_INDEX,         /*!< PCTRLA     clock source   73u */ \
    IPC_PCTRLB_INDEX,         /*!< PCTRLB     clock source   74u */ \
    IPC_PCTRLC_INDEX,         /*!< PCTRLC     clock source   75u */ \
    IPC_PCTRLD_INDEX,         /*!< PCTRLD     clock source   76u */ \
    IPC_PCTRLE_INDEX,         /*!< PCTRLE     clock source   77u */ \
    IPC_WDG0_INDEX,           /*!< WDG0       clock source   82u */ \
    IPC_TMU_INDEX,            /*!< TMU        clock source   99u */ \
    IPC_SCU_INDEX,            /*!< SCU        clock source  100u */ \
    IPC_I2C0_INDEX,           /*!< I2C0       clock source  102u */ \
    IPC_I2C1_INDEX,           /*!< I2C1       clock source  103u */ \
    IPC_UART0_INDEX,          /*!< UART0      clock source  106u */ \
    IPC_UART1_INDEX,          /*!< UART1      clock source  107u */ \
    IPC_UART2_INDEX,          /*!< UART2      clock source  108u */ \
    IPC_ACMP0_INDEX,          /*!< ACMP0      clock source  115u */ \
    IPC_STU_INDEX,            /*!< STU        clock source  119u */ \
    IPC_PCU_INDEX,            /*!< PCU        clock source  125u */ \
    IPC_RCU_INDEX,            /*!< RCU        clock source  127u */ \
    IPC_PERI_END_OF_CLOCK_INDEX,     /*!< PERI_END_OF_CLOCK clock source  128u */ \
    IPC_SIRC_INDEX,           /*!< SIRC       clock source  129u */ \
    IPC_FIRC_INDEX,           /*!< FIRC       clock source  130u */ \
    IPC_FXOSC_INDEX,          /*!< FXOSC      clock source  131u */ \
    IPC_SXOSC_INDEX,          /*!< SXOSC      clock source  132u */ \
    IPC_LPO_INDEX,            /*!< LPO        clock source  133u */ \
    IPC_END_OF_CLOCK_INDEX,     /*!< END_OF_CLOCK clock source  134u */ \
    IPC_CORE_INDEX,           /*!< CORE       clock source  135u */ \
    IPC_FAST_BUS_INDEX,       /*!< FAST_BUS   clock source  136u */ \
    IPC_SLOW_BUS_INDEX,       /*!< SLOW_BUS   clock source  137u */ \
}
/*! @brief IPC clock source select
 *  Implements peripheral_clock_source_t_Class
 */
typedef enum
{
    CLK_SRC_DISABLED        = 0x00U,            /*!< Disabled clock */
    CLK_SRC_FIRC            = 0x01U,            /*!< Fast IRC Clock */
    CLK_SRC_SIRC            = 0x02U,            /*!< Slow IRC Clock */
    CLK_SRC_FXOSC           = 0x03U,            /*!< External Fast OSC Clock */
    CLK_SRC_SXOSC           = 0x04U,            /*!< External Slow OSC Clock */
    CLK_SRC_LPO             = 0x05U,            /*!< Internal LPO Clock */
} peripheral_clock_source_t;

/* DMA module features */

/* @brief Number of DMA channels. */
#define FEATURE_DMA_CHANNELS                                (4U)
/* @brief Number of DMA virtual channels. */
#define FEATURE_DMA_VIRTUAL_CHANNELS                        (FEATURE_DMA_CHANNELS * DMA_INSTANCE_COUNT)
/* @brief Number of DMA interrupt lines. */
#define FEATURE_DMA_CHANNELS_INTERRUPT_LINES                (4U)
/* @brief Number of DMA virtual interrupt lines. */
#define FEATURE_DMA_VIRTUAL_CHANNELS_INTERRUPT_LINES        (FEATURE_DMA_CHANNELS_INTERRUPT_LINES * DMA_INSTANCE_COUNT)
/* @brief Number of DMA error interrupt lines. */
#define FEATURE_DMA_ERROR_INTERRUPT_LINES                   (1U)
/* @brief Number of DMA virtual error interrupt lines. */
#define FEATURE_DMA_VIRTUAL_ERROR_INTERRUPT_LINES           (FEATURE_DMA_ERROR_INTERRUPT_LINES * DMA_INSTANCE_COUNT)
/* @brief DMA module has error interrupt. */
#define FEATURE_DMA_HAS_ERROR_IRQ
/* @brief DMA module separate interrupt lines for each channel */
#define FEATURE_DMA_SEPARATE_IRQ_LINES_PER_CHN
/* @brief Clock name for DMA */
#define FEATURE_DMA_CLOCK_NAMES                             {DMA_CLK}
/* @brief DMA channel width based on number of CTSs: 2^N, N=4,5,... */
#define FEATURE_DMA_CH_WIDTH                                (2U)
/* @brief DMA channel to instance */
#define FEATURE_DMA_VCH_TO_INSTANCE(x) 	                    ((x) >> (uint32_t)FEATURE_DMA_CH_WIDTH)
/* @brief DMA virtual channel to channel */
#define FEATURE_DMA_VCH_TO_CH(x)                            ((x) & ((uint32_t)FEATURE_DMA_CHANNELS - 1U))
/* @brief DMA supports bus bandwidth control. */
#define FEATURE_DMA_ENGINE_STALL
/* @brief DMA supports 8 bytes transfer size. */
#define FEATURE_DMA_TRANSFER_SIZE_8B    (0U)
/* @brief DMA supports 16 bytes transfer size. */
#define FEATURE_DMA_TRANSFER_SIZE_16B    (0U)
/* @brief DMA supports 32 bytes transfer size. */
#define FEATURE_DMA_TRANSFER_SIZE_32B    (0U)
/* @brief DMA supports 64 bytes transfer size. */
#define FEATURE_DMA_TRANSFER_SIZE_64B    (0U)

/* DMAMUX module features */

/* @brief DMAMUX peripheral is available in silicon. */
#define FEATURE_DMAMUX_AVAILABLE
/* @brief Number of DMA channels. */
#define FEATURE_DMAMUX_CHANNELS                             (4U)
/* @brief Conversion from request source to the actual DMAMUX channel */
#define FEATURE_DMAMUX_REQ_SRC_TO_CH(x)                     (x)
/* @brief Mapping between request source and DMAMUX instance */
#define FEATURE_DMAMUX_REQ_SRC_TO_INSTANCE(x)               (0U)
/* @brief Conversion from DMA channel index to DMAMUX channel. */
#define FEATURE_DMAMUX_DMA_CH_TO_CH(x)                      (x)
/* @brief Conversion from DMAMUX channel DMAMUX register index. */
#define FEATURE_DMAMUX_CHN_REG_INDEX(x)                     (x)

/*!
 * @brief Structure for the DMA hardware request
 *
 * Defines the structure for the DMA hardware request collections. The user can configure the
 * hardware request into DMAMUX to trigger the DMA transfer accordingly. The index
 * of the hardware request varies according  to the to SoC.
 */
typedef enum {
    DMA_REQ_DISABLED         =   0U,     /*!< DISABLED         dma request    0u */ \
    DMA_REQ_UART0_RX         =   2U,     /*!< UART0_RX         dma request    2u */ \
    DMA_REQ_UART0_TX         =   3U,     /*!< UART0_TX         dma request    3u */ \
    DMA_REQ_UART1_RX         =   4U,     /*!< UART1_RX         dma request    4u */ \
    DMA_REQ_UART1_TX         =   5U,     /*!< UART1_TX         dma request    5u */ \
    DMA_REQ_UART2_RX         =   6U,     /*!< UART2_RX         dma request    6u */ \
    DMA_REQ_UART2_TX         =   7U,     /*!< UART2_TX         dma request    7u */ \
    DMA_REQ_I2C1_RX          =   8U,     /*!< I2C1_RX          dma request    8u */ \
    DMA_REQ_I2C1_TX          =   9U,     /*!< I2C1_TX          dma request    9u */ \
    DMA_REQ_SPI0_RX          =  14U,     /*!< SPI0_RX          dma request   14u */ \
    DMA_REQ_SPI0_TX          =  15U,     /*!< SPI0_TX          dma request   15u */ \
    DMA_REQ_SPI1_RX          =  16U,     /*!< SPI1_RX          dma request   16u */ \
    DMA_REQ_SPI1_TX          =  17U,     /*!< SPI1_TX          dma request   17u */ \
    DMA_REQ_SPI2_RX          =  18U,     /*!< SPI2_RX          dma request   18u */ \
    DMA_REQ_SPI2_TX          =  19U,     /*!< SPI2_TX          dma request   19u */ \
    DMA_REQ_eTMR1_CH0        =  20U,     /*!< eTMR1_CH0        dma request   20u */ \
    DMA_REQ_eTMR1_CH1        =  21U,     /*!< eTMR1_CH1        dma request   21u */ \
    DMA_REQ_eTMR1_CH2        =  22U,     /*!< eTMR1_CH2        dma request   22u */ \
    DMA_REQ_eTMR1_CH3        =  23U,     /*!< eTMR1_CH3        dma request   23u */ \
    DMA_REQ_eTMR1_CH4        =  24U,     /*!< eTMR1_CH4        dma request   24u */ \
    DMA_REQ_eTMR1_CH5        =  25U,     /*!< eTMR1_CH5        dma request   25u */ \
    DMA_REQ_eTMR1_CH6        =  26U,     /*!< eTMR1_CH6        dma request   26u */ \
    DMA_REQ_eTMR1_CH7        =  27U,     /*!< eTMR1_CH7        dma request   27u */ \
    DMA_REQ_eTMR2_CH0        =  28U,     /*!< eTMR2_CH0        dma request   28u */ \
    DMA_REQ_eTMR2_CH1        =  29U,     /*!< eTMR2_CH1        dma request   29u */ \
    DMA_REQ_eTMR0_CH_ORED    =  36U,     /*!< eTMR0_CH_ORED    dma request   36u */ \
    DMA_REQ_ADC0             =  42U,     /*!< ADC0             dma request   42u */ \
    DMA_REQ_I2C0_RX          =  44U,     /*!< I2C0_RX          dma request   44u */ \
    DMA_REQ_I2C0_TX          =  45U,     /*!< I2C0_TX          dma request   45u */ \
    DMA_REQ_GPIOA            =  49U,     /*!< GPIOA            dma request   49u */ \
    DMA_REQ_GPIOB            =  50U,     /*!< GPIOB            dma request   50u */ \
    DMA_REQ_GPIOC            =  51U,     /*!< GPIOC            dma request   51u */ \
    DMA_REQ_GPIOD            =  52U,     /*!< GPIOD            dma request   52u */ \
    DMA_REQ_GPIOE            =  53U,     /*!< GPIOE            dma request   53u */ \
    DMA_REQ_FlexCAN0         =  54U,     /*!< FlexCAN0         dma request   54u */ \
    DMA_REQ_ALWAYS_ON        =  63U,     /*!< ALWAYS_ON        dma request   63u */ \
} dma_request_source_t;


/* TMU module features */
/*!
 * @brief Enumeration for trigger source module of TMU
 *
 * Describes all possible inputs (trigger sources) of the TMU IP
 * This enumeration depends on the supported instances in device
 */
enum tmu_trigger_source_e
{
    TMU_TRIG_SOURCE_DISABLED                       = 0U,
    TMU_TRIG_SOURCE_VDD                            = 1U,
    TMU_TRIG_SOURCE_TMU_IN0                        = 2U,
    TMU_TRIG_SOURCE_TMU_IN1                        = 3U,
    TMU_TRIG_SOURCE_TMU_IN2                        = 4U,
    TMU_TRIG_SOURCE_TMU_IN3                        = 5U,
    TMU_TRIG_SOURCE_TMU_IN4                        = 6U,
    TMU_TRIG_SOURCE_TMU_IN5                        = 7U,
    TMU_TRIG_SOURCE_TMU_IN6                        = 8U,
    TMU_TRIG_SOURCE_TMU_IN7                        = 9U,
    TMU_TRIG_SOURCE_TMU_IN8                        = 10U,
    TMU_TRIG_SOURCE_TMU_IN9                        = 11U,
    TMU_TRIG_SOURCE_ACMP0_OUT                      = 14U,
    TMU_TRIG_SOURCE_pTMR_CH0                       = 17U,
    TMU_TRIG_SOURCE_pTMR_CH1                       = 18U,
    TMU_TRIG_SOURCE_pTMR_CH2                       = 19U,
    TMU_TRIG_SOURCE_pTMR_CH3                       = 20U,
    TMU_TRIG_SOURCE_lpTMR0                         = 21U,
    TMU_TRIG_SOURCE_eTMR0_EXT_TRIG                 = 23U,
    TMU_TRIG_SOURCE_eTMR1_EXT_TRIG                 = 25U,
    TMU_TRIG_SOURCE_eTMR2_EXT_TRIG                 = 27U,
    TMU_TRIG_SOURCE_ADC0_EOC                       = 30U,
    TMU_TRIG_SOURCE_ADC0_AWD                       = 31U,
    TMU_TRIG_SOURCE_RTC_ALARM                      = 43U,
    TMU_TRIG_SOURCE_RTC_SECOND                     = 44U,
    TMU_TRIG_SOURCE_UART0_RX_DATA                  = 49U,
    TMU_TRIG_SOURCE_UART0_TX_DATA                  = 50U,
    TMU_TRIG_SOURCE_UART0_RX_IDLE                  = 51U,
    TMU_TRIG_SOURCE_UART1_RX_DATA                  = 52U,
    TMU_TRIG_SOURCE_UART1_TX_DATA                  = 53U,
    TMU_TRIG_SOURCE_UART1_RX_IDLE                  = 54U,
    TMU_TRIG_SOURCE_I2C0_MASTER_TRIG               = 55U,
    TMU_TRIG_SOURCE_I2C0_SLAVE_TRIG                = 56U,
    TMU_TRIG_SOURCE_SPI0_FRAME                     = 59U,
    TMU_TRIG_SOURCE_SPI0_RX_DATA                   = 60U,
    TMU_TRIG_SOURCE_SPI1_FRAME                     = 61U,
    TMU_TRIG_SOURCE_SPI1_RX_DATA                   = 62U,
    TMU_TRIG_SOURCE_SW_TRIG                        = 63U,
    TMU_TRIG_SOURCE_SPI2_FRAME                     = 64U,
    TMU_TRIG_SOURCE_SPI2_RX_DATA                   = 65U,
    TMU_TRIG_SOURCE_I2C1_MASTER_TRG                = 67U,
    TMU_TRIG_SOURCE_I2C1_SLAVE_TRIG                = 68U,
    TMU_TRIG_SOURCE_UART2_RX_DATA                  = 77U,
    TMU_TRIG_SOURCE_UART2_TX_DATA                  = 78U,
    TMU_TRIG_SOURCE_UART2_RX_IDLE                  = 79U,
};

/*!
 * @brief Enumeration for target module of TMU
 *
 * Describes all possible outputs (target modules) of the TMU IP
 * This enumeration depends on the supported instances in device
 */
enum tmu_target_module_e
{
    TMU_TARGET_MODULE_TMU_OUT0                       = 4U,
    TMU_TARGET_MODULE_TMU_OUT1                       = 5U,
    TMU_TARGET_MODULE_TMU_OUT2                       = 6U,
    TMU_TARGET_MODULE_TMU_OUT3                       = 7U,
    TMU_TARGET_MODULE_TMU_OUT4                       = 8U,
    TMU_TARGET_MODULE_TMU_OUT5                       = 9U,
    TMU_TARGET_MODULE_ADC0_EXT_TRIG                  = 12U,
    TMU_TARGET_MODULE_ACMP0_SAMPLE_INPUT             = 28U,
    TMU_TARGET_MODULE_eTMR0_HWTRIG0                  = 40U,
    TMU_TARGET_MODULE_eTMR0_FAULT0                   = 41U,
    TMU_TARGET_MODULE_eTMR0_FAULT1                   = 42U,
    TMU_TARGET_MODULE_eTMR0_FAULT2                   = 43U,
    TMU_TARGET_MODULE_eTMR1_HWTRIG0                  = 44U,
    TMU_TARGET_MODULE_eTMR1_FAULT0                   = 45U,
    TMU_TARGET_MODULE_eTMR1_FAULT1                   = 46U,
    TMU_TARGET_MODULE_eTMR1_FAULT2                   = 47U,
    TMU_TARGET_MODULE_eTMR2_HWTRIG0                  = 48U,
    TMU_TARGET_MODULE_eTMR2_FAULT0                   = 49U,
    TMU_TARGET_MODULE_eTMR2_FAULT1                   = 50U,
    TMU_TARGET_MODULE_eTMR2_FAULT2                   = 51U,
    TMU_TARGET_MODULE_I2C0_TRG                       = 84U,
    TMU_TARGET_MODULE_lpTMR0_ALT0                    = 100U,
    TMU_TARGET_MODULE_I2C1_TRG                       = 108U,
};

/* @brief Constant array storing the value of all TMU output(target module) identifiers */
#define FEATURE_TMU_TARGET_MODULE                     \
    {                                                 \
    TMU_TARGET_MODULE_TMU_OUT0,                       \
    TMU_TARGET_MODULE_TMU_OUT1,                       \
    TMU_TARGET_MODULE_TMU_OUT2,                       \
    TMU_TARGET_MODULE_TMU_OUT3,                       \
    TMU_TARGET_MODULE_TMU_OUT4,                       \
    TMU_TARGET_MODULE_TMU_OUT5,                       \
    TMU_TARGET_MODULE_ADC0_EXT_TRIG,                  \
    TMU_TARGET_MODULE_ACMP0_SAMPLE_INPUT,             \
    TMU_TARGET_MODULE_eTMR0_HWTRIG0,                  \
    TMU_TARGET_MODULE_eTMR0_FAULT0,                   \
    TMU_TARGET_MODULE_eTMR0_FAULT1,                   \
    TMU_TARGET_MODULE_eTMR0_FAULT2,                   \
    TMU_TARGET_MODULE_eTMR1_HWTRIG0,                  \
    TMU_TARGET_MODULE_eTMR1_FAULT0,                   \
    TMU_TARGET_MODULE_eTMR1_FAULT1,                   \
    TMU_TARGET_MODULE_eTMR1_FAULT2,                   \
    TMU_TARGET_MODULE_eTMR2_HWTRIG0,                  \
    TMU_TARGET_MODULE_eTMR2_FAULT0,                   \
    TMU_TARGET_MODULE_eTMR2_FAULT1,                   \
    TMU_TARGET_MODULE_eTMR2_FAULT2,                   \
    TMU_TARGET_MODULE_I2C0_TRG,                       \
    TMU_TARGET_MODULE_lpTMR0_ALT0,                    \
    TMU_TARGET_MODULE_I2C1_TRG,                       \
    }

/* SPI module features */
/* @brief Initial value for state structure */
#define FEATURE_SPI_STATE_STRUCTURES_NULL {NULL, NULL, NULL}
/* @brief Has transmitter/receiver DMA enable bits. */
#define FEATURE_SPI_HAS_DMA_ENABLE (1)
/* @brief Clock indexes for SPI clock */
#define FEATURE_SPI_CLOCKS_NAMES {SPI0_CLK, SPI1_CLK, SPI2_CLK}

/* UART module features */

/* @brief Hardware flow control (RTS, CTS) is supported. */
#define FEATURE_UART_HAS_MODEM_SUPPORT (0)
/* @brief Baud rate oversampling is available. */
#define FEATURE_UART_HAS_BAUD_RATE_OVER_SAMPLING_SUPPORT (1)
/* @brief Baud rate oversampling is available. */
#define FEATURE_UART_HAS_BOTH_EDGE_SAMPLING_SUPPORT (1)
/* @brief Capacity (number of entries) of the transmit/receive FIFO (or zero if no FIFO is available). */
#define FEATURE_UART_FIFO_SIZE (4U)
/* @brief Supports two match addresses to filter incoming frames. */
#define FEATURE_UART_HAS_ADDRESS_MATCHING (1)
/* @brief Has transmitter/receiver DMA enable bits. */
#define FEATURE_UART_HAS_DMA_ENABLE (1)
/* @brief Flag clearance mask for STAT register. */
#define FEATURE_UART_INTF_REG_FLAGS_MASK (0x0000CEFFU)
/* @brief Flag clearance mask for FIFO register. */
#define FEATURE_UART_FIFO_REG_FLAGS_MASK (0x00030000U)
/* @brief Reset mask for FIFO register. */
#define FEATURE_UART_FIFO_RESET_MASK (0x00008000U)
/* @brief Default oversampling ratio. */
#define FEATURE_UART_DEFAULT_OSR (0x0FUL)
/* @brief Default baud rate divisor. */
#define FEATURE_UART_DEFAULT_DIV (0x04UL)
/* @brief Clock names for UART. */
#define UART_CLOCK_NAMES                \
    {                                   \
        UART0_CLK, UART1_CLK, UART2_CLK \
    }
/* @brief IPC index for UART module */
#define UART_IPC_INDEX                                      \
    {                                                       \
        IPC_UART0_INDEX, IPC_UART1_INDEX, IPC_UART2_INDEX   \
    }
/* @brief UART state ptr initialization */
#define FEATURE_UART_STATE_PTR  { NULL, NULL, NULL }

/* ADC module features */

/*! @brief ADC feature flag for extended number of SC1 and R registers,
 * generically named 'alias registers' */
#define FEATURE_ADC_HAS_EXTRA_NUM_REGS                    (0)
/* @brief ADC Support keep working in lp mode */
#define FEATURE_ADC_SUPPORT_LOW_POWER_KEEP                (0U)

#define NUMBER_OF_ALT_CLOCKS  ADC_CLK_ALT_1
/*! @brief ADC feature flag for defining number of external ADC channels.
 * If each ADC instance has different number of external channels, then
 * this define is set with the maximum value. */
#define FEATURE_ADC_MAX_NUM_EXT_CHANS                     (17)
#define ADC_CLOCKS                                        {ADC0_CLK}

/*! @brief ADC default clock divider from RM */
#define ADC_DEFAULT_CLOCK_DIVIDER                         (0x02U)
/*! @brief ADC default Sample Time from RM */
#define ADC_DEFAULT_SAMPLE_TIME                           (0x0CU)
/*! @brief ADC default Start Time from RM */
#define ADC_DEFAULT_START_TIME                            (0x20U)
/* @brief Max of adc clock frequency */
#define ADC_CLOCK_FREQ_MAX_RUNTIME                        (16000000u)
/* @brief Min of adc clock frequency */
#define ADC_CLOCK_FREQ_MIN_RUNTIME                        (2000000u)

/*! @brief PCU config register if need */
#define ADC_INTERNAL_PCU_CONFIG_REG                       (*(volatile uint32_t *)0x4007D010U)
/*! @brief ADC measure LDO PCU config value, 0 if hardware-connected */
#define ADC_MEASURE_LDO_PCU_CONFIG_VAL                    (1U)
/*! @brief ADC measure bandgap PCU config value, 0 if hardware-connected */
#define ADC_MEASURE_BG_PCU_CONFIG_VAL                     (8U)

/*! @brief ACMP TEST register, 0 if no configuration is required */
#define ADC_INTERNAL_ACMP_TEST_REG                         (0U)
/*! @brief ADC sense bus enable value, 0 if no configuration is required */
#define ADC_MEASURE_SB_ENABLE_VAL                          (0U)

/* CAN module features */

/* @brief FlexCAN state ptr initialization */
#define FEATURE_FLEXCAN_STATE_PTR           { NULL }
/* @brief Frames available in Rx FIFO flag shift */
#define FEATURE_CAN_RXFIFO_FRAME_AVAILABLE  (5U)
/* @brief Rx FIFO warning flag shift */
#define FEATURE_CAN_RXFIFO_WARNING          (6U)
/* @brief Rx FIFO overflow flag shift */
#define FEATURE_CAN_RXFIFO_OVERFLOW         (7U)
/* @brief The list contains definitions of the FD feature support on all instances */
#define FEATURE_CAN_INSTANCES_HAS_FD        {true}
/* @brief Has Flexible Data Rate for CAN0 */
#define FEATURE_CAN0_HAS_FD                 (1)
/* @brief Maximum number of Message Buffers supported for payload size 8 for CAN0 */
#define FEATURE_CAN0_MAX_MB_NUM             (32U)
/* @brief Has PE clock source select (bit field CAN_CTRL1[CLKSRC]). */
#define FEATURE_CAN_HAS_PE_CLKSRC_SELECT    (1)
/* @brief Has DMA enable (bit field MCR[DMA]). */
#define FEATURE_CAN_HAS_DMA_ENABLE          (1)
/* @brief Maximum number of Message Buffers supported for payload size 8 for any of the CAN instances */
#define FEATURE_CAN_MAX_MB_NUM              (32U)
/* @brief Maximum number of Message Buffers supported for payload size 8 for any of the CAN instances */
#define FEATURE_CAN_MAX_MB_NUM_ARRAY        { FEATURE_CAN0_MAX_MB_NUM }
/* @brief Has Pretending Networking mode */
#define FEATURE_CAN_HAS_PRETENDED_NETWORKING    (0)
/* @brief Has Stuff Bit Count Enable Bit */
#define FEATURE_CAN_HAS_STFCNTEN_ENABLE         (0)
/* @brief Has ISO CAN FD Enable Bit */
#define FEATURE_CAN_HAS_ISOCANFDEN_ENABLE       (1)
/* @brief Has Message Buffer Data Size Region 1 */
#define FEATURE_CAN_HAS_MBDSR1                  (0)
/* @brief Has Message Buffer Data Size Region 2 */
#define FEATURE_CAN_HAS_MBDSR2                  (0)
/* @brief DMA hardware requests for all FlexCAN instances */
#define FEATURE_CAN_DMA_REQUESTS              { DMA_REQ_FlexCAN0 }
/* @brief Max mailbox ram count */
#define FEATURE_CAN_RAM_COUNT                   (128)
/* @brief Max RXIMR count */
#define FEATURE_CAN_RXIMR_COUNT                 (32)

/* @brief Maximum number of Message Buffers IRQs */
#define FEATURE_CAN_MB_IRQS_MAX_COUNT       (2U)
/* @brief Message Buffers IRQs */
#define FEATURE_CAN_MB_IRQS                 { CAN_ORed_0_15_MB_IRQS, \
                                              CAN_ORed_16_31_MB_IRQS }
/* @brief Has Wake Up Irq channels (CAN_Wake_Up_IRQS_CH_COUNT > 0u) */
#define FEATURE_CAN_HAS_WAKE_UP_IRQ         (1)
/* @brief Has Self Wake Up mode */
#define FEATURE_CAN_HAS_SELF_WAKE_UP        (1)
/* @brief Has Flexible Data Rate */
#define FEATURE_CAN_HAS_FD                  (1)
/* @brief Has RAM ECC */
#define FEATURE_CAN_HAS_RAM_ECC             (0)
/* @brief Clock name for the PE oscillator clock source */
#define FEATURE_CAN_PE_OSC_CLK_NAME         IPC_FXOSC_CLK
/* @brief FlexCAN has Detection And Correction of Memory Errors */
#define FEATURE_CAN_HAS_MEM_ERR_DET			(0)


/* @brief Has FlexCAN Enhanced Rx FIFO mode */
#define FEATURE_CAN_HAS_ENHANCE_RX_FIFO    (STD_OFF)
/* @brief Has enhanced rx fifo feature for CAN0 */
#define FEATURE_CAN0_HAS_ENHANCE_RX_FIFO   (0)
/* @brief The list contains definitions of the rx-fifo feature support on all instances */
#define FEATURE_CAN_INSTANCES_HAS_ENHANCE_RX_FIFO        { FEATURE_CAN0_HAS_ENHANCE_RX_FIFO }
/* @brief Enhance fifo message buffer count */
#define FEATURE_CAN_ENHANCE_RX_FIFO_COUNT (20U)
/* @brief Enhance fifo message filter count */
#define FEATURE_CAN_ERFFELn_COUNT (128U)
/* @brief High-resolution time stamp feature for CAN0 */
#define FEATURE_CAN0_HAS_HR_TIMESTAMP (0U)
/* @brief High-resolution time stamp feature for all CAN */
#define FEATURE_CAN_HAS_HR_TIMESTAMP (0U)
/* @brief The list contains definitions of the High-resolution time stamp feature support on all instances */
#define FEATURE_CAN_INSTANCES_HAS_HR_TIMESTAMP           { FEATURE_CAN0_HAS_HR_TIMESTAMP }
/* lpTMR module features */

/* @brief lpTMR pulse counter input options */
#define FEATURE_lpTMR_HAS_INPUT_ALT1_SELECTION  (1U)
/* @brief Clock names for lpTMR. */
#define lpTMR_CLOCK_NAMES {lpTMR0_CLK}
/* @brief lpTMR support IPC clock selection */
#define FEATURE_lpTMR_CLKSRC_SUPPORT_IPC  (1U)

/* pTMR module features */

/* @brief Clock names for pTMR */
#define pTMR_CLOCK_NAMES {SLOW_BUS_CLK}

/* @brief pTMR instance list */
typedef enum {
    pTMR0_INST = 0,
} ptmr_instance_t;

/* @brief pTMR channels list for each instance */
typedef enum {
    pTMR0_CH0 = 0U,
    pTMR0_CH1 = 1U,
    pTMR0_CH2 = 2U,
    pTMR0_CH3 = 3U,
} ptmr_channel_t;

/* RTC module features */

typedef enum
{
    RTC_CLK_SRC_OSC_32KHZ = 0x00U,  /*!< RTC Prescaler increments using 32 KHz crystal */
    RTC_CLK_SRC_RTC_PIN = 0x01U,   /*!< RTC Prescaler increments from RTC pin*/
} rtc_clk_source_t;
/* @brief RTC has separate second irq */
#define FEATURE_RTC_HAS_SEPARATE_SECOND_IRQ  (0U)
/* @brief RTC has external 32.768 clock support */
#define FEATURE_RTC_HAS_OSC                 (FEATURE_SCU_SUPPORT_SXOSC)

/* OSIF module features */

#define FEATURE_OSIF_USE_SYSTICK                         (1)
#define FEATURE_OSIF_FREERTOS_ISR_CONTEXT_METHOD         (1) /* Cortex M device */

#if defined(USER_DEVICE_FEATURES_OVERRIDE)
#include "user_device_features_override.h"
#endif


#endif /* YTM32B1LE0_FEATURES_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/

/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file dma_driver.h
 * @version 1.5.0
 *
 * @brief DMA Driver - Public API for Direct Memory Access transfers.
 *
 * This header defines the application-level interface for the DMA peripheral.
 * It wraps the low-level hardware access layer and provides a convenient API
 * for configuring DMA channels and performing data transfers (memory-to-memory,
 * peripheral-to-memory, etc.).
 *
 * The APIs are organized into the following categories:
 *   - **Initialization & De-initialization**: Set up or tear down the DMA module.
 *   - **Channel Management**: Initialize and release individual DMA channels.
 *   - **Transfer Configuration**: Configure single-block, multi-block, loop, and ram reload transfers.
 *   - **Channel Control**: Start, stop, and trigger DMA channels.
 *   - **Source Address Configuration**: Configure source address, offset, and transfer size.
 *   - **Destination Address Configuration**: Configure destination address, offset, and transfer size.
 *   - **Transfer Loop Configuration**: Configure transfer loop byte count, trigger loop iterations,
 *     ram reload linkage, and auto-disable on completion.
 *   - **Interrupt & Callback**: Enable/disable channel interrupts and register callbacks.
 *   - **Status**: Query channel status.
 *
 * @note The DMA peripheral clock must be enabled (via clock_manager) before
 *       calling any function in this module.
 */

#ifndef DMA_DRIVER_H
#define DMA_DRIVER_H

#include "device_registers.h"
#include "status.h"
#include <stddef.h>

/*!
 * @addtogroup dma_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief Macro for the memory size needed for the software CTS.
 *
 * Software CTS is aligned to 32 bytes. We don't need a software CTS structure for the first
 * descriptor, since the configuration is pushed directly to registers.
 * To make sure the software CTS can meet the DMA module requirement regarding alignment,
 * allocate memory for the remaining descriptors with extra 31 bytes.
 */
#define SCTS_SIZE(number)           (((number) * 32U) - 1U)
#define SCTS_ADDR(address)          (((uint32_t)(address) + 31UL) & (~0x1FUL))
/*!
 * @brief Macro for accessing the least significant bit of the ERR register.
 *
 * The erroneous channels are retrieved from ERR register by subsequently right
 * shifting all the ERR bits + "AND"-ing the result with this mask.
 */
#define DMA_ERR_LSB_MASK           1U

/*!
 * @brief DMA channel interrupt source selection.
 *
 * Selects which interrupt event to enable or disable for a DMA channel.
 *
 * | Value                       | Interrupt Trigger                                     |
 * |-----------------------------|-------------------------------------------------------|
 * | DMA_CHN_ERR_INT             | Error condition occurred on the channel.               |
 * | DMA_CHN_HALF_MAJOR_LOOP_INT | Trigger loop count reached the halfway point (TCNT == TCNTRV/2). |
 * | DMA_CHN_MAJOR_LOOP_INT      | Trigger loop count reached zero (transfer complete).   |
 */
typedef enum
{
    DMA_CHN_ERR_INT = 0U,         /*!< Error interrupt */
    DMA_CHN_HALF_MAJOR_LOOP_INT,  /*!< Half trigger loop interrupt. */
    DMA_CHN_MAJOR_LOOP_INT        /*!< Complete trigger loop interrupt. */
} dma_channel_interrupt_t;


/*!
 * @brief DMA address modulo configuration.
 *
 * Controls the circular buffer size for source or destination addressing.
 * When modulo is enabled, the upper address bits are frozen so that the
 * address wraps around within a power-of-2 sized region. The buffer must
 * be aligned to its size boundary (0-modulo-size address).
 *
 * For example, DMA_MODULO_256B restricts the address to a 256-byte aligned
 * region, allowing only the lower 8 address bits to change.
 */
typedef enum
{
    DMA_MODULO_OFF = 0U,
    DMA_MODULO_2B,
    DMA_MODULO_4B,
    DMA_MODULO_8B,
    DMA_MODULO_16B,
    DMA_MODULO_32B,
    DMA_MODULO_64B,
    DMA_MODULO_128B,
    DMA_MODULO_256B,
    DMA_MODULO_512B,
    DMA_MODULO_1KB,
    DMA_MODULO_2KB,
    DMA_MODULO_4KB,
    DMA_MODULO_8KB,
    DMA_MODULO_16KB,
    DMA_MODULO_32KB,
    DMA_MODULO_64KB,
    DMA_MODULO_128KB,
    DMA_MODULO_256KB,
    DMA_MODULO_512KB,
    DMA_MODULO_1MB,
    DMA_MODULO_2MB,
    DMA_MODULO_4MB,
    DMA_MODULO_8MB,
    DMA_MODULO_16MB,
    DMA_MODULO_32MB,
    DMA_MODULO_64MB,
    DMA_MODULO_128MB,
    DMA_MODULO_256MB,
    DMA_MODULO_512MB,
    DMA_MODULO_1GB,
    DMA_MODULO_2GB
} dma_modulo_t;

/*!
 * @brief DMA data transfer size per bus access.
 *
 * Determines the number of bytes transferred in a single read or write bus cycle.
 * Source and destination can have different transfer sizes.
 *
 * | Value                  | Size   | Notes                                         |
 * |------------------------|--------|-----------------------------------------------|
 * | DMA_TRANSFER_SIZE_1B   | 1 byte | Universal; required for byte-wide peripherals.|
 * | DMA_TRANSFER_SIZE_2B   | 2 bytes| Address must be 2-byte aligned.               |
 * | DMA_TRANSFER_SIZE_4B   | 4 bytes| Address must be 4-byte aligned.               |
 * | DMA_TRANSFER_SIZE_8B   | 8 bytes| Address must be 8-byte aligned.               |
 * | DMA_TRANSFER_SIZE_16B  | 16 bytes| Not available on L/Z series.                 |
 * | DMA_TRANSFER_SIZE_32B  | 32 bytes| Not available on L/Z series.                 |
 * | DMA_TRANSFER_SIZE_64B  | 64 bytes| Address must be 64-byte aligned.             |
 *
 * @note On L/Z series devices, DMA_TRANSFER_SIZE_16B and DMA_TRANSFER_SIZE_32B
 *       are not supported.
 */
typedef enum
{
    DMA_TRANSFER_SIZE_1B = 0x0U,
    DMA_TRANSFER_SIZE_2B = 0x1U,
    DMA_TRANSFER_SIZE_4B = 0x2U,
    DMA_TRANSFER_SIZE_8B = 0x3U,
    DMA_TRANSFER_SIZE_16B = 0x4U,
    DMA_TRANSFER_SIZE_32B = 0x5U,
    DMA_TRANSFER_SIZE_64B = 0x6U,
} dma_transfer_size_t;

/*!
 * @brief DMA module user configuration.
 *
 * Holds module-level parameters passed to DMA_DRV_Init(). Controls global
 * DMA behavior that applies to all channels.
 *
 * | Field               | Type   | Description                                              |
 * |---------------------|--------|----------------------------------------------------------|
 * | haltOnError         | bool   | Halt all channels when any channel reports an error.     |
 * | eccErrorEnable      | bool   | Enable ECC error detection (device-specific).            |
 * | maxChannelForChLink | bool   | DMA errata E406002 workaround (device-specific).         |
 */
typedef struct
{
    bool haltOnError;           /*!< Any error causes the HALT bit to set. Subsequently, all
                                 service requests are ignored until the HALT bit is cleared. */
#if (defined(FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK) && FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK == 1)
    bool eccErrorEnable;       /*!< Enable ECC error detection. */
#endif /* FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK */

#if defined(FEATURE_DMA_ERRATA_E406002) && (FEATURE_DMA_ERRATA_E406002 == 1)
    bool maxChannelForChLink;  /*!< DMA errata E406002. */
#endif /* FEATURE_DMA_ERRATA_E406002 */
} dma_user_config_t;

/*!
 * @brief DMA channel status.
 *
 * Indicates the current state of a DMA channel. Returned by the channel
 * callback or by DMA_DRV_GetChannelStatus().
 *
 * | Value           | Meaning                                |
 * |-----------------|----------------------------------------|
 * | DMA_CHN_NORMAL  | Channel is idle or operating normally.  |
 * | DMA_CHN_ERROR   | An error occurred on this channel.      |
 */
typedef enum
{
    DMA_CHN_NORMAL = 0U,           /*!< DMA channel normal state. */
    DMA_CHN_ERROR                  /*!< An error occurred in the DMA channel. */
} dma_chn_status_t;

/*!
 * @brief DMA channel callback function prototype.
 *
 * Callback invoked by the DMA IRQ handler when a channel completes its
 * transfer or encounters an error. Register via DMA_DRV_InstallCallback().
 *
 * @param[in] parameter  User-defined parameter passed during callback registration.
 * @param[in] status     Channel status indicating normal completion or error.
 */
typedef void (*dma_callback_t)(void *parameter, dma_chn_status_t status);

/*!
 * @brief DMA channel runtime state.
 *
 * Internal state structure maintained by the driver for each active channel.
 * Allocated by the application and passed to DMA_DRV_ChannelInit(). The
 * memory must remain valid until DMA_DRV_ReleaseChannel() is called.
 */
typedef struct
{
    uint8_t virtChn;                     /*!< Virtual channel number. */
    dma_callback_t callback;            /*!< Callback function pointer for the DMA channel. It will
                                              be called at the DMA channel complete and DMA channel
                                              error. */
    void *parameter;                     /*!< Parameter for the callback function pointer. */
    volatile dma_chn_status_t status;   /*!< DMA channel status. */
} dma_chn_state_t;

/*!
 * @brief DMA channel initialization configuration.
 *
 * Passed to DMA_DRV_ChannelInit() to configure a single DMA channel,
 * including virtual channel number, request source, and callback.
 *
 * | Field          | Type                  | Description                                      |
 * |----------------|-----------------------|--------------------------------------------------|
 * | virtChnConfig  | uint8_t               | DMA virtual channel number.                      |
 * | source         | dma_request_source_t  | DMA request source (defined in device features).  |
 * | callback       | dma_callback_t        | Callback for transfer complete / error events.    |
 * | callbackParam  | void *                | User parameter forwarded to the callback.         |
 */
typedef struct
{
    uint8_t virtChnConfig;                   /*!< DMA virtual channel number */
    dma_request_source_t source;             /*!< Selects the source of the DMA request for this channel */
    dma_callback_t callback;                /*!< Callback that will be registered for this channel */
    void *callbackParam;                    /*!< Parameter passed to the channel callback */
} dma_channel_config_t;

/*!
 * @brief DMA transfer direction type.
 *
 * Specifies the source and destination type (memory or peripheral) for a
 * DMA transfer. Affects how the driver configures address offsets: peripheral
 * addresses use zero offset (fixed register), while memory addresses are
 * auto-incremented.
 */
typedef enum
{
    DMA_TRANSFER_PERIPH2MEM = 0U,   /*!< Transfer from peripheral to memory */
    DMA_TRANSFER_MEM2PERIPH,        /*!< Transfer from memory to peripheral */
    DMA_TRANSFER_MEM2MEM,           /*!< Transfer from memory to memory */
    DMA_TRANSFER_PERIPH2PERIPH      /*!< Transfer from peripheral to peripheral */
} dma_transfer_type_t;

/*!
 * @brief Ram reload (scatter-gather) list entry.
 *
 * Describes one memory block in a ram reload transfer chain. An array of
 * these structures is passed to DMA_DRV_ConfigRamReloadTransfer() for both
 * source and destination.
 *
 * | Field   | Type                 | Description                                   |
 * |---------|----------------------|-----------------------------------------------|
 * | address | uint32_t             | Start address of the data buffer.              |
 * | length  | uint32_t             | Number of bytes in the buffer (ignored if peripheral). |
 * | type    | dma_transfer_type_t  | Transfer direction (M2M, M2P, P2M, P2P).      |
 */
typedef struct
{
    uint32_t address;           /*!< Address of buffer. */
    uint32_t length;            /*!< Length of buffer. */
    dma_transfer_type_t type;  /*!< Type of the DMA transfer */
} dma_ram_reload_list_t;

/*!
 * @brief DMA module runtime state.
 *
 * Internal state structure that the driver uses to track all virtual channel
 * allocations. Allocated by the application and passed to DMA_DRV_Init().
 * The memory must remain valid until DMA_DRV_Deinit() is called.
 */
typedef struct
{
    dma_chn_state_t *volatile virtChnState[(uint32_t) FEATURE_DMA_VIRTUAL_CHANNELS];   /*!< Pointer array storing channel state. */
    
#if defined(FEATURE_DMA_ERRATA_E406002) && (FEATURE_DMA_ERRATA_E406002 == 1)
    bool maxChannelForChLinkState;                                                     /* DMA ERRATA E406002, the max channel link state */
#endif
} dma_state_t;

/*!
 * @brief DMA loop transfer configuration.
 *
 * Configures the transfer loop and trigger loop behavior for loop-mode
 * transfers. Pass a pointer to this structure in the @c loopTransferConfig
 * field of dma_transfer_config_t when using DMA_DRV_ConfigLoopTransfer().
 *
 * | Field                      | Type     | Description                                                        |
 * |----------------------------|----------|--------------------------------------------------------------------|
 * | triggerLoopIterationCount   | uint32_t | Number of trigger loop iterations.                                 |
 * | srcOffsetEnable             | bool     | Apply transfer loop offset to source address on loop completion.   |
 * | dstOffsetEnable             | bool     | Apply transfer loop offset to destination address on loop completion. |
 * | triggerLoopOffset           | int32_t  | Signed offset applied to address after transfer loop completes.    |
 * | transferLoopChnLinkEnable   | bool     | Enable channel linking on transfer loop completion.                |
 * | transferLoopChnLinkNumber   | uint8_t  | Channel to link on transfer loop completion.                       |
 * | triggerLoopChnLinkEnable    | bool     | Enable channel linking on trigger loop completion.                 |
 * | triggerLoopChnLinkNumber    | uint8_t  | Channel to link on trigger loop completion.                        |
 */
typedef struct
{
    uint32_t triggerLoopIterationCount;   /*!< Number of trigger loop iterations. */
    bool srcOffsetEnable;                 /*!< Selects whether the transfer loop offset is applied to the
                                                 source address upon transfer loop completion. */
    bool dstOffsetEnable;                 /*!< Selects whether the transfer loop offset is applied to the
                                                 destination address upon transfer loop completion. */
    int32_t triggerLoopOffset;            /*!< Sign-extended offset applied to the source or destination address
                                                 to form the next-state value after the transfer loop completes. */
    bool transferLoopChnLinkEnable;       /*!< Enables channel-to-channel linking on transfer loop complete. */
    uint8_t transferLoopChnLinkNumber;    /*!< The number of the next channel to be started by DMA
                                                 engine when transfer loop completes. */
    bool triggerLoopChnLinkEnable;        /*!< Enables channel-to-channel linking on trigger loop complete. */
    uint8_t triggerLoopChnLinkNumber;     /*!< The number of the next channel to be started by DMA
                                                 engine when trigger loop completes. */
} dma_loop_transfer_config_t;

/*!
 * @brief DMA transfer configuration.
 *
 * Complete transfer descriptor used by DMA_DRV_PushConfigToReg(),
 * DMA_DRV_PushConfigToSCTS(), DMA_DRV_ConfigLoopTransfer(), and
 * DMA_DRV_ConfigRamReloadTransfer(). Defines source/destination addresses,
 * transfer sizes, offsets, modulo settings, and optional loop configuration.
 *
 * | Field                 | Type                           | Description                                         |
 * |-----------------------|--------------------------------|-----------------------------------------------------|
 * | srcAddr               | uint32_t                       | Source buffer start address.                         |
 * | destAddr              | uint32_t                       | Destination buffer start address.                    |
 * | srcTransferSize       | dma_transfer_size_t            | Bytes per source bus read.                           |
 * | destTransferSize      | dma_transfer_size_t            | Bytes per destination bus write.                     |
 * | srcOffset             | int16_t                        | Signed address increment after each source read.     |
 * | destOffset            | int16_t                        | Signed address increment after each destination write.|
 * | srcLastAddrAdjust     | int32_t                        | Adjustment to source address after trigger loop completes. |
 * | destLastAddrAdjust    | int32_t                        | Adjustment to destination address (only when ram reload is disabled). |
 * | srcModulo             | dma_modulo_t                   | Source circular buffer modulo.                       |
 * | destModulo            | dma_modulo_t                   | Destination circular buffer modulo.                  |
 * | transferLoopByteCount | uint32_t                       | Bytes transferred per transfer loop iteration.       |
 * | ramReloadEnable       | bool                           | Enable ram reload (scatter-gather) mode.             |
 * | ramReloadNextDescAddr | uint32_t                       | Address of the next CTS descriptor (32-byte aligned).|
 * | interruptEnable       | bool                           | Enable interrupt on trigger loop completion.         |
 * | loopTransferConfig    | dma_loop_transfer_config_t *   | Pointer to loop configuration (NULL if unused).      |
 */
typedef struct
{
    uint32_t srcAddr;                                   /*!< Memory address pointing to the source data. */
    uint32_t destAddr;                                  /*!< Memory address pointing to the destination data. */
    dma_transfer_size_t srcTransferSize;                /*!< Source data transfer size. */
    dma_transfer_size_t destTransferSize;               /*!< Destination data transfer size. */
    int16_t srcOffset;                                  /*!< Sign-extended offset applied to the current source address
                                                           to form the next-state value as each source read/write
                                                           is completed. */
    int16_t destOffset;                                 /*!< Sign-extended offset applied to the current destination
                                                           address to form the next-state value as each source
                                                           read/write is completed. */
    int32_t srcLastAddrAdjust;                          /*!< Last source address adjustment. */
    int32_t destLastAddrAdjust;                         /*!< Last destination address adjustment. Note here it is only
                                                           valid when ram reload feature is not enabled. */
    dma_modulo_t srcModulo;                             /*!< Source address modulo. */
    dma_modulo_t destModulo;                            /*!< Destination address modulo. */
    uint32_t transferLoopByteCount;                    /*!< Number of bytes to be transferred in each service request
                                                           of the channel. */
    bool ramReloadEnable;                               /*!< Enable ram reload feature. */
    uint32_t ramReloadNextDescAddr;                     /*!< The address of the next descriptor to be used, when
                                                           ram reload feature is enabled.
                                                           Note: this value is not used when ram reload
                                                           feature is disabled. */
    bool interruptEnable;                               /*!< Enable the interrupt request when the trigger loop
                                                           count completes */
    dma_loop_transfer_config_t *loopTransferConfig;     /*!< Pointer to loop transfer configuration structure
                                                           (defines transfer/trigger loop attributes)
                                                           Note: this field is only used when transfer loop mapping is
                                                           enabled from DMA configuration. */
} dma_transfer_config_t;

/*!
 * @brief Software CTS (Channel Transfer Structure) descriptor.
 *
 * Mirrors the hardware CTS register layout (32 bytes). Used for ram reload
 * (scatter-gather) transfers where multiple transfer descriptors are chained
 * in memory. Allocate using the SCTS_SIZE() macro and align with SCTS_ADDR().
 */
typedef struct
{
    uint32_t SADDR;             /*!< Source address */
    int16_t SOFF;               /*!< Source offset */
    uint16_t ATTR;              /*!< Transfer attributes */
    uint32_t BCNT;              /*!< One loop transfer byte count */
    int32_t STO;                /*!< Source trigger loop offset */
    uint32_t DADDR;             /*!< Destination address */
    int16_t DOFF;               /*!< Destination offset */
    uint16_t TCNT;              /*!< Trigger loop transfer count */
    int32_t RAM_LOAD_ADDR;      /*!< RAM load address */
    uint16_t CSR;               /*!< Control/status register */
    uint16_t TCNTRV;            /*!< Trigger loop counter value */
} dma_software_cts_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization & De-initialization
 * @brief Functions for setting up and tearing down the DMA module.
 * @{
 */

/*!
 * @brief Initialize the DMA module with user-provided configuration.
 *
 * Resets the DMA peripheral to its default state, then applies the module-level
 * settings from @a userConfig. Initializes the internal state structure for
 * virtual channel allocation and optionally configures a set of channels.
 *
 * @param[in]  dmaState       Pointer to the module runtime state structure.
 *                            The caller allocates this memory; the driver
 *                            populates it. Must remain valid until DMA_DRV_Deinit().
 * @param[in]  userConfig     Pointer to the module configuration (halt-on-error, etc.).
 * @param[in]  chnStateArray  Array of pointers to per-channel state structures.
 * @param[in]  chnConfigArray Array of pointers to per-channel configurations.
 * @param[in]  chnCount       Number of channels to initialize (array length).
 * @return Execution status.
 * @retval STATUS_SUCCESS  Initialization completed successfully.
 * @retval STATUS_ERROR    Invalid parameter or channel allocation failure.
 *
 * @pre  The DMA peripheral clock must be enabled via the clock_manager.
 * @post The DMA module is configured and the specified channels are ready
 *       for transfer configuration.
 */
status_t DMA_DRV_Init(dma_state_t *dmaState,
                      const dma_user_config_t *userConfig,
                      dma_chn_state_t *const chnStateArray[],
                      const dma_channel_config_t *const chnConfigArray[],
                      uint32_t chnCount);

/*!
 * @brief De-initialize the DMA module, releasing all resources.
 *
 * Resets the DMA peripheral to its default state and releases all channel
 * state structures. After calling this function the module may be
 * re-initialized with DMA_DRV_Init().
 *
 * @return Execution status.
 * @retval STATUS_SUCCESS  De-initialization completed successfully.
 * @retval STATUS_ERROR    Module was not initialized.
 *
 * @note This function does NOT disable the DMA peripheral clock. The caller
 *       is responsible for clock management.
 */
status_t DMA_DRV_Deinit(void);

/*! @} */

/*!
 * @name Channel Management
 * @brief Functions for allocating and releasing individual DMA channels.
 * @{
 */

/*!
 * @brief Initialize a single DMA channel.
 *
 * Allocates the virtual channel, configures the DMA request source via
 * DMAMUX (if available), and registers the user callback.
 *
 * @param[in]  dmaChannelState   Pointer to per-channel state structure. The
 *                               caller allocates this; memory must remain valid
 *                               until DMA_DRV_ReleaseChannel().
 * @param[in]  dmaChannelConfig  Pointer to channel configuration (virtual channel
 *                               number, request source, callback).
 * @return Execution status.
 * @retval STATUS_SUCCESS  Channel initialized successfully.
 * @retval STATUS_ERROR    Channel already allocated or invalid parameter.
 *
 * @pre  DMA_DRV_Init() must have been called.
 */
status_t DMA_DRV_ChannelInit(dma_chn_state_t *dmaChannelState,
                             const dma_channel_config_t *dmaChannelConfig);

/*!
 * @brief Release a DMA channel.
 *
 * Stops any in-progress transfer on the channel, disables the channel
 * interrupt, and frees the virtual channel for reuse.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @return Execution status.
 * @retval STATUS_SUCCESS  Channel released successfully.
 * @retval STATUS_ERROR    Channel was not initialized.
 */
status_t DMA_DRV_ReleaseChannel(uint8_t virtualChannel);

/*! @} */

/*!
 * @name Transfer Configuration
 * @brief Functions for configuring DMA transfer descriptors: single-block,
 *        multi-block, loop, and ram reload (scatter-gather) modes.
 * @{
 */

/*!
 * @brief Copy a transfer configuration to the hardware CTS registers.
 *
 * Writes all fields from @a cts directly to the channel's hardware CTS
 * registers. The channel should be stopped before calling this function.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] cts             Pointer to the transfer configuration structure.
 */
void DMA_DRV_PushConfigToReg(uint8_t virtualChannel,
                             const dma_transfer_config_t *cts);

/*!
 * @brief Copy a transfer configuration to a software CTS structure.
 *
 * Populates a software CTS descriptor in memory from the given transfer
 * configuration. The software CTS address can then be used as the next
 * descriptor in a ram reload (scatter-gather) chain.
 *
 * @param[in]  config  Pointer to the transfer configuration structure.
 * @param[out] scts    Pointer to the software CTS structure to populate.
 */
void DMA_DRV_PushConfigToSCTS(const dma_transfer_config_t *config,
                              dma_software_cts_t *scts);

/*!
 * @brief Configure a single-block DMA transfer.
 *
 * Sets up a one-shot transfer of a contiguous data buffer. The source and
 * destination address offsets are derived automatically from @a transferSize
 * so that the buffer is traversed without gaps. For peripheral transfers,
 * the peripheral side uses a zero offset (fixed register address).
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] type            Transfer direction (M2M, P2M, M2P, P2P).
 * @param[in] srcAddr         Source start address (register or memory).
 * @param[in] destAddr        Destination start address (register or memory).
 * @param[in] transferSize    Bytes per bus access (source and destination share
 *                            this size). Must match the peripheral data register
 *                            width for peripheral transfers.
 * @param[in] dataBufferSize  Total number of bytes to transfer.
 * @return Execution status.
 * @retval STATUS_SUCCESS  Transfer configured and started.
 * @retval STATUS_ERROR    Invalid parameter or channel not initialized.
 *
 * @warning For peripheral transfers, @a transferSize must equal the peripheral
 *          data register width (e.g. 1 byte for an 8-bit UART data register,
 *          4 bytes for a 32-bit SPI FIFO).
 */
status_t DMA_DRV_ConfigSingleBlockTransfer(uint8_t virtualChannel,
                                           dma_transfer_type_t type,
                                           uint32_t srcAddr,
                                           uint32_t destAddr,
                                           dma_transfer_size_t transferSize,
                                           uint32_t dataBufferSize);

/*!
 * @brief Configure a multi-block DMA transfer.
 *
 * Divides a contiguous buffer into multiple equal-sized blocks. Each block
 * is transferred upon a separate DMA request (one trigger loop iteration per
 * block). Address offsets are derived from @a transferSize automatically.
 *
 * @param[in] virtualChannel         DMA virtual channel number.
 * @param[in] type                   Transfer direction (M2M, P2M, M2P, P2P).
 * @param[in] srcAddr                Source start address.
 * @param[in] destAddr               Destination start address.
 * @param[in] transferSize           Bytes per bus access. Must match the
 *                                   peripheral data register width for
 *                                   peripheral transfers.
 * @param[in] blockSize              Bytes per block (one transfer loop).
 * @param[in] blockCount             Number of blocks (trigger loop count).
 * @param[in] disableReqOnCompletion If true, the DMA request is auto-cleared
 *                                   when all blocks are transferred.
 * @return Execution status.
 * @retval STATUS_SUCCESS  Transfer configured and started.
 * @retval STATUS_ERROR    Invalid parameter or channel not initialized.
 *
 * @warning For peripheral transfers, @a transferSize must equal the peripheral
 *          data register width.
 */
status_t DMA_DRV_ConfigMultiBlockTransfer(uint8_t virtualChannel,
                                          dma_transfer_type_t type,
                                          uint32_t srcAddr,
                                          uint32_t destAddr,
                                          dma_transfer_size_t transferSize,
                                          uint32_t blockSize,
                                          uint32_t blockCount,
                                          bool disableReqOnCompletion);

/*!
 * @brief Configure a loop-mode DMA transfer.
 *
 * Sets up a transfer with extended loop control (transfer loop offsets,
 * channel linking on loop completion). Requires transfer loop mapping to
 * be enabled in the DMA module configuration.
 *
 * @param[in] virtualChannel   DMA virtual channel number.
 * @param[in] transferConfig   Pointer to the transfer configuration. The
 *                             @c loopTransferConfig field must point to a
 *                             valid dma_loop_transfer_config_t structure.
 * @return Execution status.
 * @retval STATUS_SUCCESS  Transfer configured successfully.
 * @retval STATUS_ERROR    Transfer loop mapping not enabled or invalid parameter.
 *
 * @pre Transfer loop mapping must be enabled (set in dma_user_config_t).
 */
status_t DMA_DRV_ConfigLoopTransfer(uint8_t virtualChannel,
                                    const dma_transfer_config_t *transferConfig);

/*!
 * @brief Configure a ram reload (scatter-gather) DMA transfer chain.
 *
 * Builds a linked chain of CTS descriptors in memory. Each entry in the
 * source/destination lists describes one data block. The DMA engine
 * automatically reloads the next descriptor after completing each block;
 * an interrupt fires only when the last block completes.
 *
 * @param[in]  virtualChannel     DMA virtual channel number.
 * @param[out] scts               Array of software CTS structures. Must be
 *                                allocated with at least SCTS_SIZE(@a ctsCount)
 *                                bytes and aligned to 32 bytes via SCTS_ADDR().
 *                                The first descriptor is pushed directly to
 *                                hardware registers; the remaining descriptors
 *                                are stored in this array.
 * @param[in]  transferSize       Bytes per bus access.
 * @param[in]  bytesOnEachRequest Bytes transferred per DMA request.
 * @param[in]  srcList            Source block descriptors (address, length, type).
 *                                If source is a peripheral, length is ignored.
 * @param[in]  destList           Destination block descriptors. For M2M transfers,
 *                                destination lengths must match source lengths.
 *                                If destination is a peripheral, length is ignored.
 * @param[in]  ctsCount           Number of blocks in the ram reload chain.
 * @return Execution status.
 * @retval STATUS_SUCCESS  Transfer chain configured successfully.
 * @retval STATUS_ERROR    Alignment error, invalid parameter, or channel not initialized.
 *
 * @pre  DMA_DRV_ChannelInit() must have been called for this channel.
 * @note The @a scts buffer must be 32-byte aligned. Use the SCTS_SIZE() and
 *       SCTS_ADDR() macros for correct allocation and alignment.
 */
status_t DMA_DRV_ConfigRamReloadTransfer(uint8_t virtualChannel,
                                         dma_software_cts_t *scts,
                                         dma_transfer_size_t transferSize,
                                         uint32_t bytesOnEachRequest,
                                         const dma_ram_reload_list_t *srcList,
                                         const dma_ram_reload_list_t *destList,
                                         uint8_t ctsCount);

/*!
 * @brief Cancel the currently executing DMA transfer.
 *
 * Stops the active transfer after the current read/write sequence completes.
 * Optionally treats the cancellation as an error condition.
 *
 * @param[in] error  If true, the cancellation is logged as a channel error.
 *                   If false, the channel retires normally.
 */
void DMA_DRV_CancelTransfer(bool error);

/*! @} */

/*!
 * @name Channel Control
 * @brief Functions for starting, stopping, and triggering DMA channels.
 * @{
 */

/*!
 * @brief Start a DMA channel by enabling its hardware request.
 *
 * After the channel is started, it will respond to DMA requests from the
 * configured request source.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @return Execution status.
 * @retval STATUS_SUCCESS  Channel started successfully.
 * @retval STATUS_ERROR    Channel not initialized.
 */
status_t DMA_DRV_StartChannel(uint8_t virtualChannel);

/*!
 * @brief Stop a DMA channel by disabling its hardware request.
 *
 * The channel will no longer respond to DMA requests after this call.
 * Any in-progress transfer loop completes normally before the channel idles.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @return Execution status.
 * @retval STATUS_SUCCESS  Channel stopped successfully.
 * @retval STATUS_ERROR    Channel not initialized.
 */
status_t DMA_DRV_StopChannel(uint8_t virtualChannel);

/*!
 * @brief Configure the DMA request source for a channel.
 *
 * Routes a DMA request source to the specified virtual channel via DMAMUX.
 * Available request sources are defined in the device-specific features header.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] request         DMA request source number.
 * @return Execution status.
 * @retval STATUS_SUCCESS      Request source configured.
 * @retval STATUS_UNSUPPORTED  DMAMUX not available on this device.
 */
status_t DMA_DRV_SetChannelRequestAndTrigger(uint8_t virtualChannel,
                                             uint8_t request);

/*!
 * @brief Clear all CTS registers for a channel to zero.
 *
 * Resets the hardware CTS registers of the specified channel to their default
 * state (all zeros). Useful before reconfiguring a channel.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 */
void DMA_DRV_ClearCTS(uint8_t virtualChannel);

/*!
 * @brief Issue a software-triggered DMA request for a channel.
 *
 * Triggers one transfer loop iteration via software, bypassing the hardware
 * request source. The channel must have been configured and started.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 */
void DMA_DRV_TriggerSwRequest(uint8_t virtualChannel);

/*! @} */

/*!
 * @name Source Address Configuration
 * @brief Functions for configuring the source address, offset, transfer size,
 *        and last-address adjustment of a DMA channel.
 * @{
 */

/*!
 * @brief Set the source address for a DMA channel.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] address         Source memory or register address.
 */
void DMA_DRV_SetSrcAddr(uint8_t virtualChannel, uint32_t address);

/*!
 * @brief Set the source address signed offset for a DMA channel.
 *
 * Sign-extended offset added to the source address after each source read
 * to form the next read address.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] offset          Signed address offset (bytes).
 */
void DMA_DRV_SetSrcOffset(uint8_t virtualChannel, int16_t offset);

/*!
 * @brief Set the source data read chunk size for a DMA channel.
 *
 * Determines how many bytes are read in a single bus access from the
 * source (1/2/4/8/16/32/64 bytes).
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] size            Source transfer size enumeration.
 */
void DMA_DRV_SetSrcReadChunkSize(uint8_t virtualChannel,
                                 dma_transfer_size_t size);

/*!
 * @brief Set the source address last adjustment for a DMA channel.
 *
 * Signed adjustment value added to the source address when the trigger loop
 * count completes. Typically used to reset the source address to its initial
 * value (set to negative total transfer size) for circular operation.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] adjust          Signed adjustment value (bytes).
 */
void DMA_DRV_SetSrcLastAddrAdjustment(uint8_t virtualChannel,
                                      int32_t adjust);

/*! @} */

/*!
 * @name Destination Address Configuration
 * @brief Functions for configuring the destination address, offset, transfer
 *        size, and last-address adjustment of a DMA channel.
 * @{
 */

/*!
 * @brief Set the destination address for a DMA channel.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] address         Destination memory or register address.
 */
void DMA_DRV_SetDestAddr(uint8_t virtualChannel, uint32_t address);

/*!
 * @brief Set the destination address signed offset for a DMA channel.
 *
 * Sign-extended offset added to the destination address after each
 * destination write to form the next write address.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] offset          Signed address offset (bytes).
 */
void DMA_DRV_SetDestOffset(uint8_t virtualChannel, int16_t offset);

/*!
 * @brief Set the destination data write chunk size for a DMA channel.
 *
 * Determines how many bytes are written in a single bus access to the
 * destination (1/2/4/8/16/32/64 bytes).
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] size            Destination transfer size enumeration.
 */
void DMA_DRV_SetDestWriteChunkSize(uint8_t virtualChannel,
                                   dma_transfer_size_t size);

/*!
 * @brief Set the destination address last adjustment for a DMA channel.
 *
 * Signed adjustment value added to the destination address when the trigger
 * loop count completes. Typically used to reset the destination address for
 * circular operation. Only effective when ram reload is disabled.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] adjust          Signed adjustment value (bytes).
 */
void DMA_DRV_SetDestLastAddrAdjustment(uint8_t virtualChannel,
                                       int32_t adjust);

/*! @} */

/*!
 * @name Transfer Loop Configuration
 * @brief Functions for configuring transfer loop byte count, trigger loop
 *        iterations, ram reload linkage, and auto-disable on completion.
 * @{
 */

/*!
 * @brief Set the transfer loop byte count for a DMA channel.
 *
 * Specifies the number of bytes transferred per transfer loop iteration
 * (one DMA service request). Must be a multiple of the source/destination
 * transfer size.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] nbytes          Bytes per transfer loop iteration.
 */
void DMA_DRV_SetTransferLoopBlockSize(uint8_t virtualChannel,
                                      uint32_t nbytes);

/*!
 * @brief Set the trigger loop iteration count for a DMA channel.
 *
 * Specifies how many trigger loop iterations (DMA requests) are needed to
 * complete the full transfer. Each iteration transfers the number of bytes
 * configured by DMA_DRV_SetTransferLoopBlockSize().
 *
 * @param[in] virtualChannel   DMA virtual channel number.
 * @param[in] majorLoopCount   Number of trigger loop iterations.
 */
void DMA_DRV_SetTriggerLoopIterationCount(uint8_t virtualChannel,
                                          uint32_t majorLoopCount);

/*!
 * @brief Get the remaining trigger loop iteration count.
 *
 * Returns the number of trigger loop iterations still pending for the
 * specified channel.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @return Number of trigger loop iterations remaining.
 */
uint32_t DMA_DRV_GetRemainingTriggerIterationsCount(uint8_t virtualChannel);

/*!
 * @brief Set the ram reload link address for a DMA channel.
 *
 * Configures the address of the next CTS descriptor loaded from memory when
 * the trigger loop completes. The address must be 32-byte aligned
 * (0-modulo-32); a configuration error is reported otherwise.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] nextCTSAddr     32-byte aligned address of the next CTS descriptor.
 *
 * @warning The ram reload address must be aligned to a 32-byte boundary.
 */
void DMA_DRV_SetRamReloadLink(uint8_t virtualChannel,
                              uint32_t nextCTSAddr);

/*!
 * @brief Enable or disable auto-clear of the DMA request on trigger loop completion.
 *
 * When @a disable is true, the DMA hardware automatically clears the channel's
 * request when the trigger loop count reaches zero, preventing further transfers.
 * When false, the request remains active and new triggers continue to start
 * transfers (useful for continuous/circular operation).
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] disable         true = auto-clear request on completion;
 *                            false = keep request active.
 */
void DMA_DRV_DisableRequestsOnTransferComplete(uint8_t virtualChannel,
                                               bool disable);

/*! @} */

/*!
 * @name Interrupt & Callback
 * @brief Functions for enabling/disabling channel interrupts and registering
 *        user callback functions.
 * @{
 */

/*!
 * @brief Enable or disable a channel interrupt source.
 *
 * Controls error, half-trigger-loop, and trigger-loop-complete interrupts
 * for the specified channel.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] intSrc          Interrupt source to configure.
 * @param[in] enable          true = enable; false = disable.
 */
void DMA_DRV_ConfigureInterrupt(uint8_t virtualChannel,
                                dma_channel_interrupt_t intSrc,
                                bool enable);

/*!
 * @brief Register or unregister a callback for a DMA channel.
 *
 * The callback is invoked from the DMA IRQ handler when the channel
 * completes its transfer or encounters an error. Pass NULL as @a callback
 * to unregister a previously installed callback.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @param[in] callback        Pointer to the callback function, or NULL to
 *                            unregister.
 * @param[in] parameter       User parameter forwarded to the callback.
 * @return Execution status.
 * @retval STATUS_SUCCESS  Callback registered successfully.
 * @retval STATUS_ERROR    Channel not initialized.
 */
status_t DMA_DRV_InstallCallback(uint8_t virtualChannel,
                                 dma_callback_t callback,
                                 void *parameter);

/*! @} */

/*!
 * @name Status
 * @brief Functions for querying DMA channel status.
 * @{
 */

/*!
 * @brief Get the current status of a DMA channel.
 *
 * @param[in] virtualChannel  DMA virtual channel number.
 * @return Channel status (DMA_CHN_NORMAL or DMA_CHN_ERROR).
 */
dma_chn_status_t DMA_DRV_GetChannelStatus(uint8_t virtualChannel);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* DMA_DRIVER_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/


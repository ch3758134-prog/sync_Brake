/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file dma_hw_access.h
 * @version 1.5.0
 *
 * @brief DMA Hardware Access Layer.
 *
 * This header provides low-level inline accessor functions and register
 * manipulation routines for the DMA peripheral. Functions are organized into:
 *   - Module Initialization
 *   - Module Configuration & Control
 *   - CTS Source Configuration
 *   - CTS Destination Configuration
 *   - CTS Ram Reload & Interrupt
 *   - CTS Transfer Loop & Trigger
 *   - DMAMUX
 *   - Utility
 *
 * @note This is an internal layer used by the DMA Driver (dma_driver.h/c).
 *       Application code should use the DMA_DRV_* APIs from dma_driver.h.
 */

#ifndef DMA_HW_ACCESS_H
#define DMA_HW_ACCESS_H

#include <stdint.h>
#include <stdbool.h>
#include "dma_driver.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#ifdef FEATURE_DMA_ENGINE_STALL
/*!
 * @brief DMA engine stall duration after each read/write operation.
 *
 * Defines the number of system clock cycles the DMA engine pauses after
 * completing each read/write access. Increasing the stall duration reduces
 * DMA bus utilization and allows more bandwidth for the CPU or other
 * bus masters.
 */
typedef enum
{
    DMA_ENGINE_STALL_0_CYCLES = 0,
    DMA_ENGINE_STALL_4_CYCLES = 2,
    DMA_ENGINE_STALL_8_CYCLES = 3
} dma_engine_stall_t;
#endif

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Module Initialization
 * @brief Functions for initializing the DMA module and cancelling transfers.
 * @{
 */

/*!
 * @brief Initialize the DMA module to its default state.
 *
 * Resets all DMA control registers and CTS entries to zero.
 *
 * @param[in] base  Pointer to the DMA peripheral base address.
 *
 * @pre The DMA peripheral clock must be enabled.
 */
void DMA_Init(DMA_Type *base);

/*!
 * @brief Cancel the remaining data transfer gracefully.
 *
 * Stops the executing channel and forces the transfer loop to finish.
 * The cancellation takes effect after the last write of the current
 * read/write sequence. The CX bit clears itself after the cancel has
 * been honored. The channel retires normally as if the trigger loop
 * had completed.
 *
 * @param[in] base  Pointer to the DMA peripheral base address.
 *
 * @note The channel is retired normally; no error is flagged.
 */
void DMA_CancelTransfer(DMA_Type *base);

/*!
 * @brief Cancel the remaining data transfer and flag it as an error.
 *
 * Stops the executing channel and forces the transfer loop to finish.
 * The cancellation takes effect after the last write of the current
 * read/write sequence. The CX bit clears itself after the cancel has
 * been honored. Unlike DMA_CancelTransfer(), this function additionally
 * treats the cancellation as an error condition, setting the appropriate
 * error status bits.
 *
 * @param[in] base  Pointer to the DMA peripheral base address.
 *
 * @warning The error status bits must be cleared manually after calling
 *          this function.
 */
void DMA_CancelTransferWithError(DMA_Type *base);

/*!
 * @brief Configure whether the DMA module halts on an error condition.
 *
 * When enabled, an error causes the HALT bit to be set, and all subsequent
 * service requests are ignored until the HALT bit is cleared.
 *
 * @param[in] base         Pointer to the DMA peripheral base address.
 * @param[in] haltOnError  true = halt the DMA module when an error occurs;
 *                         false = do not halt on error.
 */
static inline void DMA_SetHaltOnErrorCmd(DMA_Type *base, bool haltOnError)
{
    uint32_t regValTemp;
    regValTemp = base->CTRL;
    regValTemp &= ~(DMA_CTRL_POE_MASK);
    regValTemp |= DMA_CTRL_POE(haltOnError ? 1UL : 0UL);
    base->CTRL = regValTemp;
}

#if (defined(FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK) && FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK == 1)
/*!
 * @brief Enable or disable ECC error detection for DMA RAM.
 *
 * When enabled, the DMA module checks for ECC errors on internal SRAM
 * accesses and reports them via the error interrupt mechanism.
 *
 * @param[in] base    Pointer to the DMA peripheral base address.
 * @param[in] enable  true = enable ECC error detection;
 *                    false = disable ECC error detection.
 *
 * @note This function is only available when
 *       FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK is defined and set to 1.
 */

static inline void DMA_SetEccErrorCheckCmd(DMA_Type *base, bool enable)
{
    uint32_t regValTemp;
    regValTemp = base->EIE;
    regValTemp |= DMA_EIE_ECCEN(enable ? 1UL : 0UL) |
                  DMA_EIE_TAG(0x5AUL);
    base->EIE = regValTemp;
}
#endif /* FEATURE_DMA_SUPPORT_ECC_ERROR_CHECK */

/*! @} */

/*!
 * @name Module Configuration & Control
 * @brief Functions for configuring DMA transfer loop mapping, channel requests,
 *        error interrupts, and interrupt status flags.
 * @{
 */
/*!
 * @brief Enable or disable the transfer loop offset mapping feature.
 *
 * When enabled, the BCNT register is redefined to include individual enable
 * fields and the BCNT field. The individual enable fields allow the transfer
 * loop offset to be applied to the source address, the destination address,
 * or both. The BCNT field width is reduced when either offset is enabled.
 *
 * @param[in] base    Pointer to the DMA peripheral base address.
 * @param[in] enable  true = enable transfer loop mapping;
 *                    false = disable transfer loop mapping.
 *
 * @pre This function must be called before configuring any transfer loop
 *      offset via DMA_CTSSetTransferLoopOffset().
 */
static inline void DMA_SetTransferLoopMappingCmd(DMA_Type *base, bool enable)
{
    uint32_t regValTemp;
    regValTemp = base->CTRL;
    regValTemp &= ~(DMA_CTRL_LOEN_MASK);
    regValTemp |= DMA_CTRL_LOEN(enable ? 1UL : 0UL);
    base->CTRL = regValTemp;
}

/*!
 * @brief Enable or disable the error interrupt for a specific DMA channel.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number.
 * @param[in] enable   true = enable error interrupt for the channel;
 *                     false = disable error interrupt for the channel.
 */
void DMA_SetErrorIntCmd(DMA_Type *base, uint8_t channel, bool enable);

/*!
 * @brief Get the DMA error interrupt status flags for all channels.
 *
 * Returns a 32-bit bitmask where each bit corresponds to one DMA channel.
 * A bit set to 1 indicates that an error condition occurred on that channel.
 *
 * @param[in] base  Pointer to the DMA peripheral base address.
 *
 * @return Bitmask of error interrupt flags. Bit n corresponds to channel n:
 *         - 1 = error occurred on channel n.
 *         - 0 = no error on channel n.
 */
static inline uint32_t DMA_GetErrorIntStatusFlag(const DMA_Type *base)
{
    return base->CHEIF;
}

/*!
 * @brief Clear the error interrupt status flag for a specific DMA channel.
 *
 * Writes a 1 to the corresponding bit position in the CHEIF register to
 * clear the error flag. Other channel flags are not affected.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 */
static inline void DMA_ClearErrorIntStatusFlag(DMA_Type *base, uint8_t channel)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    base->CHEIF = (0x01UL << channel);
}

/*!
 * @brief Enable or disable the DMA hardware request for a specific channel.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number.
 * @param[in] enable   true = enable the DMA request for the channel;
 *                     false = disable the DMA request for the channel.
 */
void DMA_SetDmaRequestCmd(DMA_Type *base, uint8_t channel, bool enable);

/*!
 * @brief Clear the done status flag for a specific DMA channel.
 *
 * Writes a 1 to the corresponding bit position in the DONE register to
 * clear the done flag. Other channel flags are not affected.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 */
static inline void DMA_ClearDoneStatusFlag(DMA_Type *base, uint8_t channel)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    base->DONE = 0x01UL << channel;
}

/*!
 * @brief Start a DMA transfer by software trigger on a specific channel.
 *
 * Sets the START bit in the channel's CTS CSR register, initiating a
 * service request for the channel.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 */
static inline void DMA_TriggerChannelStart(DMA_Type *base, uint8_t channel)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    base->CTS[channel].CSR |= DMA_CTS_CSR_START_MASK;
}

/*!
 * @brief Clear the trigger loop done interrupt status flag for a DMA channel.
 *
 * Writes a 1 to the corresponding bit in the CHTLDIF register to clear
 * the interrupt flag. A dummy read-back is performed to ensure the write
 * completes before the function returns.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 */
static inline void DMA_ClearIntStatusFlag(DMA_Type *base, uint8_t channel)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    base->CHTLDIF = 0x01UL << channel;
    (void)base->CHTLDIF;
}

/*!
 * @brief Get the trigger loop done interrupt status flag for a DMA channel.
 *
 * Reads the CHTLDIF register to determine whether the trigger loop done
 * interrupt flag is set for the specified channel.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 *
 * @return Interrupt status:
 * @retval true   The trigger loop done interrupt flag is set for the channel.
 * @retval false  The trigger loop done interrupt flag is not set.
 */
static inline bool DMA_GetIntStatusFlag(DMA_Type *base, uint8_t channel)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    return ((base->CHTLDIF & (0x01UL << channel)) != 0U);
}

/*! @} */

/*!
 * @name CTS Source Configuration
 * @brief Functions for configuring CTS source address, offset, transfer
 *        attributes, byte count, transfer loop offset, and last source
 *        address adjustment.
 * @{
 */

/*!
 * @brief Reset all CTS register fields to zero for a DMA channel.
 *
 * Clears every field of the hardware CTS entry associated with the
 * specified channel, returning it to the power-on default state.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number.
 */
void DMA_CTSClearReg(DMA_Type *base, uint8_t channel);

#ifdef FEATURE_DMA_ENGINE_STALL

/*!
 * @brief Configure the DMA engine stall duration after each read/write.
 *
 * Sets the bandwidth control (BWC) field in the CTS CSR register,
 * causing the DMA engine to stall for the specified number of cycles
 * after each read/write access. This can be used to throttle DMA bus
 * utilization.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] cycles   Number of stall cycles (see dma_engine_stall_t).
 *
 * @note This function is only available when FEATURE_DMA_ENGINE_STALL
 *       is defined.
 */
static inline void DMA_CTSSetEngineStall(DMA_Type *base, uint8_t channel, dma_engine_stall_t cycles)
{
#ifdef DEV_ERROR_DETECT
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    uint16_t regValTemp;
    regValTemp = base->CTS[channel].CSR;
    regValTemp &= ~(DMA_CTS_CSR_BWC_MASK);
    regValTemp |= (uint16_t)DMA_CTS_CSR_BWC(cycles);
    base->CTS[channel].CSR = regValTemp;
}

#endif

/*!
 * @brief Set the source address for a CTS entry.
 *
 * Writes the starting source memory address into the CTS SADDR register
 * for the specified DMA channel.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] address  Source memory address for the DMA transfer.
 */
static inline void DMA_CTSSetSrcAddr(DMA_Type *base, uint8_t channel, uint32_t address)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    base->CTS[channel].SADDR = address;
}

/*!
 * @brief Set the source address signed offset for a CTS entry.
 *
 * Configures the sign-extended offset that is added to the current source
 * address after each source read to form the next-state address.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] offset   Signed offset in bytes applied after each source read.
 */
static inline void DMA_CTSSetSrcOffset(DMA_Type *base, uint8_t channel, int16_t offset)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    base->CTS[channel].SOFF = (uint16_t) offset;
}

/*!
 * @brief Set the transfer attributes (modulo, transfer size) for a DMA channel.
 *
 * Configures the source/destination modulo and transfer size fields in the
 * CTS TCR register. The modulo feature restricts the address range to a
 * power-of-2 region, enabling circular buffer operation. The transfer size
 * determines the width of each bus read/write (1/2/4/16/32 bytes).
 *
 * @param[in] base              Pointer to the DMA peripheral base address.
 * @param[in] channel           DMA channel number.
 * @param[in] srcModulo         Source address modulo. Specifies the number of
 *                              lower address bits allowed to change, enabling
 *                              a circular source data queue. Set to
 *                              DMA_MODULO_OFF to disable.
 * @param[in] destModulo        Destination address modulo. Same behavior as
 *                              srcModulo but applied to the destination address.
 * @param[in] srcTransferSize   Source data read transfer width.
 * @param[in] destTransferSize  Destination data write transfer width.
 */
void DMA_CTSSetAttribute(
    DMA_Type *base, uint8_t channel,
    dma_modulo_t srcModulo, dma_modulo_t destModulo,
    dma_transfer_size_t srcTransferSize, dma_transfer_size_t destTransferSize);

/*!
 * @brief Set the source data read transfer size for a CTS entry.
 *
 * Configures the width of each source bus read (1/2/4/16/32 bytes) in the
 * CTS TCR register.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] size     Source transfer size (see dma_transfer_size_t).
 */
static inline void DMA_CTSSetSrcTransferSize(DMA_Type *base, uint8_t channel, dma_transfer_size_t size)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    uint16_t regValTemp;
    regValTemp = base->CTS[channel].TCR;
    regValTemp &= (uint16_t) (~(DMA_CTS_TCR_SSIZE_MASK));
    regValTemp |= (uint16_t) (DMA_CTS_TCR_SSIZE((uint16_t) size));
    base->CTS[channel].TCR = regValTemp;
}

/*!
 * @brief Set the destination data write transfer size for a CTS entry.
 *
 * Configures the width of each destination bus write (1/2/4/16/32 bytes) in
 * the CTS TCR register.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] size     Destination transfer size (see dma_transfer_size_t).
 */
static inline void DMA_CTSSetDestTransferSize(DMA_Type *base, uint8_t channel, dma_transfer_size_t size)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    uint16_t regValTemp;
    regValTemp = base->CTS[channel].TCR;
    regValTemp &= (uint16_t) (~(DMA_CTS_TCR_DSIZE_MASK));
    regValTemp |= (uint16_t) (DMA_CTS_TCR_DSIZE((uint16_t) size));
    base->CTS[channel].TCR = regValTemp;
}

/*!
 * @brief Set the byte count (nbytes) transferred per transfer loop iteration.
 *
 * Configures how many bytes are transferred in each service request of the
 * channel. The exact register layout depends on whether transfer loop
 * offset mapping is enabled.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number.
 * @param[in] nbytes   Number of bytes transferred per transfer loop iteration.
 *
 * @pre Transfer loop mapping (EMLM) must be configured via
 *      DMA_SetTransferLoopMappingCmd() before calling this function.
 */
void DMA_CTSSetNbytes(DMA_Type *base, uint8_t channel, uint32_t nbytes);

/*!
 * @brief Enable or disable the source transfer loop offset for a CTS entry.
 *
 * When enabled, the transfer loop offset value is applied to the source
 * address upon each transfer loop completion.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] enable   true = apply transfer loop offset to source address;
 *                     false = do not apply.
 *
 * @pre Transfer loop mapping (EMLM) must be enabled via
 *      DMA_SetTransferLoopMappingCmd() before calling this function;
 *      otherwise this function has no effect.
 */
static inline void DMA_CTSSetSrcMinorLoopOffsetCmd(DMA_Type *base, uint8_t channel, bool enable)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    if (((base->CTRL >> DMA_CTRL_LOEN_SHIFT) & 1U) != 0U)
    {
        uint32_t regValTemp;
        regValTemp = base->CTS[channel].BCNT.LOEN;
        regValTemp &= ~(DMA_CTS_BCNT_LOEN_SLOE_MASK);
        regValTemp |= DMA_CTS_BCNT_LOEN_SLOE(enable ? 1UL : 0UL);
        base->CTS[channel].BCNT.LOEN = regValTemp;
    }
}

/*!
 * @brief Enable or disable the destination transfer loop offset for a CTS entry.
 *
 * When enabled, the transfer loop offset value is applied to the destination
 * address upon each transfer loop completion.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] enable   true = apply transfer loop offset to destination address;
 *                     false = do not apply.
 *
 * @pre Transfer loop mapping (EMLM) must be enabled via
 *      DMA_SetTransferLoopMappingCmd() before calling this function;
 *      otherwise this function has no effect.
 */
static inline void DMA_CTSSetDestMinorLoopOffsetCmd(DMA_Type *base, uint8_t channel, bool enable)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    if (((base->CTRL >> DMA_CTRL_LOEN_SHIFT) & 1U) != 0U)
    {
        uint32_t regValTemp;
        regValTemp = base->CTS[channel].BCNT.LOEN;
        regValTemp &= ~(DMA_CTS_BCNT_LOEN_DLOE_MASK);
        regValTemp |= DMA_CTS_BCNT_LOEN_DLOE(enable ? 1UL : 0UL);
        base->CTS[channel].BCNT.LOEN = regValTemp;
    }
}

/*!
 * @brief Set the transfer loop offset value for a CTS entry.
 *
 * Configures the signed offset that is applied to the source and/or
 * destination address at the completion of each transfer loop iteration.
 * The offset is only applied if the corresponding source or destination
 * transfer loop offset enable bit is set.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number.
 * @param[in] offset   Signed transfer loop offset value in bytes.
 *
 * @pre Transfer loop mapping (EMLM) must be enabled via
 *      DMA_SetTransferLoopMappingCmd() before calling this function;
 *      otherwise this function has no effect.
 * @pre At least one of source or destination transfer loop offset must be
 *      enabled via DMA_CTSSetSrcMinorLoopOffsetCmd() or
 *      DMA_CTSSetDestMinorLoopOffsetCmd().
 */
void DMA_CTSSetTransferLoopOffset(DMA_Type *base, uint8_t channel, int32_t offset);

/*!
 * @brief Set the last source address adjustment applied after trigger loop completion.
 *
 * Configures the signed adjustment value that is added to the source address
 * when the trigger loop count is exhausted. This can be used to restore the
 * source address to its initial value or to advance it to the next data
 * structure.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] size     Signed adjustment value in bytes.
 */
static inline void DMA_CTSSetSrcLastAdjust(DMA_Type *base, uint8_t channel, int32_t size)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    base->CTS[channel].STO = (uint32_t) size;
}

/*! @} */

/*!
 * @name CTS Destination Configuration
 * @brief Functions for configuring CTS destination address, offset, and last
 *        destination address adjustment.
 * @{
 */

/*!
 * @brief Set the destination address for a CTS entry.
 *
 * Writes the starting destination memory address into the CTS DADDR register
 * for the specified DMA channel.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] address  Destination memory address for the DMA transfer.
 */
static inline void DMA_CTSSetDestAddr(DMA_Type *base, uint8_t channel, uint32_t address)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    base->CTS[channel].DADDR = address;
}

/*!
 * @brief Set the destination address signed offset for a CTS entry.
 *
 * Configures the sign-extended offset that is added to the current
 * destination address after each destination write to form the next-state
 * address.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] offset   Signed offset in bytes applied after each destination write.
 */
static inline void DMA_CTSSetDestOffset(DMA_Type *base, uint8_t channel, int16_t offset)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    base->CTS[channel].DOFF = (uint16_t) offset;
}

/*!
 * @brief Set the last destination address adjustment applied after trigger loop completion.
 *
 * Configures the signed adjustment value that is added to the destination
 * address when the trigger loop count is exhausted. This can be used to
 * restore the destination address to its initial value or to advance it to
 * the next data structure.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] adjust   Signed adjustment value in bytes.
 *
 * @warning When ram reload (scatter/gather) is enabled on the same channel,
 *          the DTO field is repurposed as the reload address. Do not use
 *          this function in that case; use DMA_CTSSetRamReloadLink() instead.
 */
static inline void DMA_CTSSetDestLastAdjust(DMA_Type *base, uint8_t channel, int32_t adjust)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    base->CTS[channel].DTO_RLD.DTO = (uint32_t) adjust;
}

/*! @} */

/*!
 * @name CTS Ram Reload & Interrupt
 * @brief Functions for configuring CTS ram reload (scatter/gather), channel
 *        linking on trigger loop completion, DMA request disable, and
 *        half-complete / complete interrupt generation.
 * @{
 */

/*!
 * @brief Configure the ram reload link address for a CTS entry.
 *
 * Enables the ram reload (scatter/gather) feature and sets the address of
 * the next CTS descriptor to be loaded into this channel when the trigger
 * loop count completes. The address points to the beginning of a
 * 0-modulo-32-byte region containing the next transfer CTS.
 *
 * @param[in] base         Pointer to the DMA peripheral base address.
 * @param[in] channel      DMA channel number.
 * @param[in] nextCTSAddr  32-bit address of the next CTS descriptor. Must be
 *                         aligned to a 32-byte boundary.
 *
 * @warning A configuration error is reported if the address is not
 *          0-modulo-32-byte aligned.
 */
void DMA_CTSSetRamReloadLink(DMA_Type *base, uint8_t channel, uint32_t nextCTSAddr);

/*!
 * @brief Enable or disable the ram reload (scatter/gather) feature for a CTS entry.
 *
 * When enabled, the CTS descriptor pointed to by the DTO_RLD register is
 * automatically loaded into the channel upon trigger loop completion.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] enable   true = enable ram reload; false = disable ram reload.
 */
static inline void DMA_CTSSetRamReloadCmd(DMA_Type *base, uint8_t channel, bool enable)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    uint16_t regValTemp;
    regValTemp = base->CTS[channel].CSR;
    regValTemp &= (uint16_t) ~(DMA_CTS_CSR_RLDEN_MASK);
    regValTemp |= (uint16_t) DMA_CTS_CSR_RLDEN(enable ? 1UL : 0UL);
    base->CTS[channel].CSR = regValTemp;
}

/*!
 * @brief Configure the trigger loop channel link for a CTS entry.
 *
 * When enabled, after the trigger loop counter is exhausted the DMA engine
 * initiates a channel service request on the linked channel by setting its
 * START bit.
 *
 * @param[in] base              Pointer to the DMA peripheral base address.
 * @param[in] channel           DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] majorLinkChannel  Channel number to link on trigger loop completion.
 * @param[in] enable            true = enable trigger loop channel link;
 *                              false = disable trigger loop channel link.
 */
static inline void DMA_CTSSetChannelTriggerLink(DMA_Type *base, uint8_t channel, uint32_t majorLinkChannel, bool enable)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    uint16_t regValTemp;
    regValTemp = base->CTS[channel].CSR;
    regValTemp &= (uint16_t) ~(DMA_CTS_CSR_TLKCH_MASK);
    regValTemp |= (uint16_t) DMA_CTS_CSR_TLKCH(majorLinkChannel);
    base->CTS[channel].CSR = regValTemp;
    regValTemp = base->CTS[channel].CSR;
    regValTemp &= (uint16_t) ~(DMA_CTS_CSR_TLKEN_MASK);
    regValTemp |= (uint16_t) DMA_CTS_CSR_TLKEN(enable ? 1UL : 0UL);
    base->CTS[channel].CSR = regValTemp;
}

/*!
 * @brief Configure automatic DMA request disable after trigger loop completion.
 *
 * When enabled (disable = true), the DMA hardware automatically clears the
 * corresponding channel's DMA request enable bit when the trigger loop count
 * reaches zero, preventing further service requests until the request is
 * re-enabled.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] disable  true = auto-clear the DMA request after trigger loop done;
 *                     false = keep the DMA request enabled after trigger loop done.
 */
static inline void DMA_CTSSetDisableDmaRequestAfterCTSDoneCmd(DMA_Type *base, uint8_t channel, bool disable)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    uint16_t regValTemp;
    regValTemp = base->CTS[channel].CSR;
    regValTemp &= (uint16_t) ~(DMA_CTS_CSR_DREQ_MASK);
    regValTemp |= (uint16_t) DMA_CTS_CSR_DREQ(disable ? 1UL : 0UL);
    base->CTS[channel].CSR = regValTemp;
}

/*!
 * @brief Enable or disable the half-complete interrupt for a CTS entry.
 *
 * When enabled, the channel generates an interrupt request when the current
 * trigger loop count reaches the halfway point (TCNT == TCNTRV >> 1). This
 * is useful for double-buffered schemes or other scenarios where the
 * application needs an early indication of transfer progress.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] enable   true = enable half-complete interrupt;
 *                     false = disable half-complete interrupt.
 */
static inline void DMA_CTSSetMajorHalfCompleteIntCmd(DMA_Type *base, uint8_t channel, bool enable)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    uint16_t regValTemp;
    regValTemp = base->CTS[channel].CSR;
    regValTemp &= (uint16_t) ~(DMA_CTS_CSR_THDINT_MASK);
    regValTemp |= (uint16_t) DMA_CTS_CSR_THDINT(enable ? 1UL : 0UL);
    base->CTS[channel].CSR = regValTemp;
}

/*!
 * @brief Enable or disable the trigger loop complete interrupt for a CTS entry.
 *
 * When enabled, the channel generates an interrupt request when the trigger
 * loop count reaches zero, indicating that the entire transfer is complete.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number (must be < FEATURE_DMA_CHANNELS).
 * @param[in] enable   true = enable trigger loop complete interrupt;
 *                     false = disable trigger loop complete interrupt.
 */
static inline void DMA_CTSSetMajorCompleteIntCmd(DMA_Type *base, uint8_t channel, bool enable)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    uint16_t regValTemp;
    regValTemp = base->CTS[channel].CSR;
    regValTemp &= (uint16_t) ~(DMA_CTS_CSR_TDINT_MASK);
    regValTemp |= (uint16_t) DMA_CTS_CSR_TDINT(enable ? 1UL : 0UL);
    base->CTS[channel].CSR = regValTemp;
}

/*! @} */

/*!
 * @name CTS Transfer Loop & Trigger
 * @brief Functions for configuring transfer loop channel linking and trigger
 *        loop iteration counts.
 * @{
 */

/*!
 * @brief Configure the transfer loop channel link for a CTS entry.
 *
 * When enabled, upon completion of each transfer loop iteration the DMA
 * engine initiates a service request on the linked channel.
 *
 * @param[in] base         Pointer to the DMA peripheral base address.
 * @param[in] channel      DMA channel number.
 * @param[in] linkChannel  Channel number to link on each transfer loop completion.
 * @param[in] enable       true = enable transfer loop channel link;
 *                         false = disable transfer loop channel link.
 */
void DMA_CTSSetChannelLoopLink(DMA_Type *base, uint8_t channel, uint32_t linkChannel, bool enable);

/*!
 * @brief Set the trigger loop count for a DMA channel.
 *
 * Writes the trigger loop iteration count into the CTS. The internal
 * register layout depends on whether the transfer loop channel link is
 * enabled, so the link setting must be configured before calling this
 * function.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number.
 * @param[in] count    Trigger loop count (number of trigger loop iterations).
 *
 * @pre The transfer loop channel link must be configured via
 *      DMA_CTSSetChannelLoopLink() before calling this function.
 */
void DMA_CTSSetTriggerCount(DMA_Type *base, uint8_t channel, uint32_t count);

/*!
 * @brief Get the current trigger loop iteration count for a DMA channel.
 *
 * Reads the remaining trigger loop iteration count from the CTS. The
 * register layout depends on the transfer loop channel link setting, which
 * is accounted for internally.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMA channel number.
 *
 * @return Remaining trigger loop iteration count.
 */
uint32_t DMA_CTSGetCurrentTriggerCount(const DMA_Type *base, uint8_t channel);

/*! @} */

/*!
 * @name DMAMUX
 * @brief Functions for initializing the DMAMUX module and routing DMA request
 *        sources to DMA channels.
 * @{
 */

#ifdef FEATURE_DMAMUX_AVAILABLE

/*!
 * @brief Initialize the DMAMUX module to its reset state.
 *
 * Resets all DMAMUX channel source selections to their default values.
 *
 * @param[in] base  Pointer to the DMA peripheral base address.
 *
 * @pre The DMAMUX peripheral clock must be enabled.
 */
void DMAMUX_Init(DMA_Type *base);

#endif


#ifdef FEATURE_DMAMUX_AVAILABLE

/*!
 * @brief Set the DMA request source for a DMAMUX channel.
 *
 * Selects which peripheral DMA request source is routed to the specified
 * DMA channel. The available source numbers are defined in the
 * device-specific <MCU>_Features.h header.
 *
 * @param[in] base     Pointer to the DMA peripheral base address.
 * @param[in] channel  DMAMUX channel number (must be < FEATURE_DMAMUX_CHANNELS).
 * @param[in] source   DMA request source number.
 */
static inline void DMAMUX_SetChannelSource(DMA_Type *base, uint8_t channel, uint8_t source)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMAMUX_CHANNELS);
#endif
    base->CHMUX[channel] = source;
}

#endif

/*! @} */

/*!
 * @name Utility
 * @brief Helper functions for obtaining DMA register base addresses.
 * @{
 */

/*!
 * @brief Get the DMA register base address for a given instance.
 *
 * Returns a pointer to the DMA_Type register structure for the specified
 * DMA peripheral instance.
 *
 * @param[in] instance  DMA peripheral instance number.
 *
 * @return Pointer to the DMA peripheral base address (DMA_Type).
 */
DMA_Type *DMA_DRV_GetDmaRegBaseAddr(uint32_t instance);

/*! @} */

#if defined(__cplusplus)
}
#endif

#endif /* DMA_HW_ACCESS_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/



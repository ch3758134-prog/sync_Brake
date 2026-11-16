/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file dma_hw_access.c
 * @version 1.5.0
 */

 /*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 2985 Rule 2.2 : This operation is redundant. The value of the result is always that 
 *                        of the left-hand operand.
 *
 */
 

#include "dma_hw_access.h"
#include "interrupt_manager.h"

/*!
 * @brief Initializes DMA module to known state.
 */
void DMA_Init(DMA_Type *base)
{
    uint8_t i;
    /* Clear the bit of CR register */
    uint32_t regValTemp;
    regValTemp = base->CTRL;
    regValTemp &= ~(DMA_CTRL_CLM_MASK);
    regValTemp &= ~(DMA_CTRL_CX_MASK);
    regValTemp &= ~(DMA_CTRL_ECX_MASK);
    regValTemp &= ~(DMA_CTRL_DBGDIS_MASK);
    regValTemp &= ~(DMA_CTRL_LOEN_MASK);
    base->CTRL = regValTemp;
    for (i = 0; i < FEATURE_DMA_CHANNELS; i++)
    {
        DMA_CTSClearReg(base, i);
    }
}

/*!
 * @brief Cancels the remaining data transfer.
 */
void DMA_CancelTransfer(DMA_Type *base)
{
    uint32_t regValTemp;
    regValTemp = base->CTRL;
    regValTemp &= ~(DMA_CTRL_CX_MASK);
    regValTemp |= DMA_CTRL_CX(1U);
    base->CTRL = regValTemp;
    while (((base->CTRL & DMA_CTRL_CX_MASK) >> DMA_CTRL_CX_SHIFT) != 0UL)
    {}
}

/*!
 * @brief Cancels the remaining data transfer and treat it as error.
 */
void DMA_CancelTransferWithError(DMA_Type *base)
{
    uint32_t regValTemp;
    regValTemp = base->CTRL;
    regValTemp &= ~(DMA_CTRL_ECX_MASK);
    regValTemp |= DMA_CTRL_ECX(1U);
    base->CTRL = regValTemp;
    while (((base->CTRL & DMA_CTRL_ECX_MASK) >> DMA_CTRL_ECX_SHIFT) != 0UL)
    {}
}

/*!
 * @brief Enable/Disable error interrupt for channels.
 */
void DMA_SetErrorIntCmd(DMA_Type *base, uint8_t channel, bool enable)
{
    SDK_ENTER_CRITICAL();
    if (enable)
    {
        base->CHEIE |= 0x01UL << channel;
    } else
    {
        base->CHEIE &= ~(0x01UL << channel);
    }
    SDK_EXIT_CRITICAL();
}

/*!
 * @brief Enable/Disable dma request for channel or all channels.
 */
void DMA_SetDmaRequestCmd(DMA_Type *base, uint8_t channel, bool enable)
{
#if defined(DMA_REQSET_CH_MASK)
    /* DMA support bset enable channel request */
    if (enable)
    {
            base->REQSET = (uint32_t)0x01U << channel;
    } else
    {
            base->REQCLR = (uint32_t)0x01U << channel;
    }
#else
    /* Halt DMA before changing the REQEN register */
    SDK_ENTER_CRITICAL();
    base->CTRL |= ((uint32_t)0x1U << 17U);
    int32_t timeout = 50;
    /* Wait DMA active done */
    while ((base->CTRL & DMA_CTRL_ACTIVE_MASK) != 0UL)
    {
        timeout--;
        if (timeout <= 0)
        {
            break;
        }
    }
    if (enable)
    {
        base->REQEN |= (0x01UL << channel);
    } else
    {
        base->REQEN &= ~(0x01UL << channel);
    }
    /* Resume DMA */
    base->CTRL &= ~((uint32_t)0x1U << 17UL);
    SDK_EXIT_CRITICAL();
#endif
}

/*!
 * @brief Set registers to 0 for hardware CTS of DMA channel.
 */
void DMA_CTSClearReg(DMA_Type *base, uint8_t channel)
{
    base->CTS[channel].BCNT.BCNT = 0U;
    base->CTS[channel].SADDR = 0x5A5A5A5AUL;
    base->CTS[channel].SOFF = 0;
    base->CTS[channel].TCR = 0U;
    base->CTS[channel].STO = 0;
    base->CTS[channel].DADDR = 0x5A5A5A5AUL;
    base->CTS[channel].DOFF = 0;
    base->CTS[channel].TCNT.LKDIS = 0U;
    base->CTS[channel].DTO_RLD.DTO = 0;
    base->CTS[channel].CSR = 0U;
    base->CTS[channel].TCNTRV = 0U;
}

/*!
 * @brief Configures the transfer attribute for DMA channel.
 */
void DMA_CTSSetAttribute(
    DMA_Type *base, uint8_t channel,
    dma_modulo_t srcModulo, dma_modulo_t destModulo,
    dma_transfer_size_t srcTransferSize, dma_transfer_size_t destTransferSize)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    uint16_t regValTemp;
    regValTemp = (uint16_t) (DMA_CTS_TCR_SMOD(srcModulo) | DMA_CTS_TCR_SSIZE(srcTransferSize));
    regValTemp |= (uint16_t) (DMA_CTS_TCR_DMOD(destModulo) | DMA_CTS_TCR_DSIZE(destTransferSize)); /* PRQA S 2985 */
    base->CTS[channel].TCR = regValTemp;
}

/*!
 * @brief Configures the nbytes for DMA channel.
 */
void DMA_CTSSetNbytes(DMA_Type *base, uint8_t channel, uint32_t nbytes)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif

    if (((base->CTRL & DMA_CTRL_LOEN_MASK) >> DMA_CTRL_LOEN_SHIFT) != 0UL)
    {
        bool mlOffNo = false;
        if (((base->CTS[channel].BCNT.LODIS & DMA_CTS_BCNT_LODIS_SLOE_MASK) >> DMA_CTS_BCNT_LODIS_SLOE_SHIFT) == 0UL)
        {
            if (((base->CTS[channel].BCNT.LODIS & DMA_CTS_BCNT_LODIS_DLOE_MASK) >> DMA_CTS_BCNT_LODIS_DLOE_SHIFT) ==
                0UL)
            {
                base->CTS[channel].BCNT.LODIS = (nbytes & DMA_CTS_BCNT_LODIS_BCNT_MASK);
                mlOffNo = true;
            }
        }
        if (!mlOffNo)
        {
            uint32_t regValTemp;
            regValTemp = base->CTS[channel].BCNT.LOEN;
            regValTemp &= ~(DMA_CTS_BCNT_LOEN_BCNT_MASK);
            regValTemp |= DMA_CTS_BCNT_LOEN_BCNT(nbytes);  /* PRQA S 2985 */
            base->CTS[channel].BCNT.LOEN = regValTemp;
        }
    } else
    {
        base->CTS[channel].BCNT.BCNT = nbytes;
    }
}

/*!
 * @brief Configures the transfer loop offset for the CTS.
 */
void DMA_CTSSetTransferLoopOffset(DMA_Type *base, uint8_t channel, int32_t offset)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif

    if (((base->CTRL & DMA_CTRL_LOEN_MASK) >> DMA_CTRL_LOEN_SHIFT) != 0UL)
    {
        bool mlOffNo = false;
        if (((base->CTS[channel].BCNT.LODIS & DMA_CTS_BCNT_LODIS_SLOE_MASK) >> DMA_CTS_BCNT_LODIS_SLOE_SHIFT) != 0UL)
        {
            mlOffNo = true;
        }
        if (((base->CTS[channel].BCNT.LODIS & DMA_CTS_BCNT_LODIS_DLOE_MASK) >> DMA_CTS_BCNT_LODIS_DLOE_SHIFT) != 0UL)
        {
            mlOffNo = true;
        }
        if (mlOffNo)
        {
            uint32_t regValTemp;
            regValTemp = base->CTS[channel].BCNT.LOEN;
            regValTemp &= ~(DMA_CTS_BCNT_LOEN_OFFSET_MASK);
            regValTemp |= DMA_CTS_BCNT_LOEN_OFFSET(offset);
            base->CTS[channel].BCNT.LOEN = regValTemp;
        }
    }
}

/*!
 * @brief Configures the memory address of the next CTS, in Scatter/Gather mode.
 */
void DMA_CTSSetRamReloadLink(DMA_Type *base, uint8_t channel, uint32_t nextCTSAddr)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    base->CTS[channel].DTO_RLD.RLD = nextCTSAddr;
}

/*!
 * @brief Sets the channel transfer loop link for the CTS.
 */
void DMA_CTSSetChannelLoopLink(
    DMA_Type *base, uint8_t channel, uint32_t linkChannel, bool enable)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
    DEV_ASSERT(linkChannel < FEATURE_DMA_CHANNELS);
#endif

    uint16_t regValTemp;
    regValTemp = base->CTS[channel].TCNTRV;
    regValTemp &= (uint16_t) ~(DMA_CTS_TCNT_LKEN_LKEN_MASK);
    regValTemp |= (uint16_t) DMA_CTS_TCNT_LKEN_LKEN(enable ? 1UL : 0UL); /* PRQA S 2985 */
    base->CTS[channel].TCNTRV = regValTemp;
    regValTemp = base->CTS[channel].TCNT.LKEN;
    regValTemp &= (uint16_t) ~(DMA_CTS_TCNT_LKEN_LKEN_MASK);
    regValTemp |= (uint16_t) DMA_CTS_TCNT_LKEN_LKEN(enable ? 1UL : 0UL);  /* PRQA S 2985 */
    base->CTS[channel].TCNT.LKEN = regValTemp;

    if (enable)
    {
        regValTemp = base->CTS[channel].TCNTRV;
        regValTemp &= (uint16_t) ~(DMA_CTS_TCNT_LKEN_LKCH_MASK);
        regValTemp |= (uint16_t) DMA_CTS_TCNT_LKEN_LKCH(linkChannel);
        base->CTS[channel].TCNTRV = regValTemp;

        regValTemp = base->CTS[channel].TCNT.LKEN;
        regValTemp &= (uint16_t) ~(DMA_CTS_TCNT_LKEN_LKCH_MASK);
        regValTemp |= (uint16_t) DMA_CTS_TCNT_LKEN_LKCH(linkChannel);
        base->CTS[channel].TCNT.LKEN = regValTemp;
    }
}

/*!
 * @brief Sets the trigger loop iteration count according to transfer loop channel link setting.
 */
void DMA_CTSSetTriggerCount(DMA_Type *base, uint8_t channel, uint32_t count)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    uint16_t regValTemp;
    if ((base->CTS[channel].TCNTRV & DMA_CTS_TCNT_LKDIS_LKEN_MASK) == DMA_CTS_TCNT_LKDIS_LKEN_MASK)
    {
        regValTemp = base->CTS[channel].TCNTRV;
        regValTemp &= (uint16_t) ~(DMA_CTS_TCNT_LKEN_TCNT_MASK);
        regValTemp |= (uint16_t) DMA_CTS_TCNT_LKEN_TCNT(count);  /* PRQA S 2985 */
        base->CTS[channel].TCNTRV = regValTemp;

        regValTemp = base->CTS[channel].TCNT.LKEN;
        regValTemp &= (uint16_t) ~(DMA_CTS_TCNT_LKEN_TCNT_MASK);
        regValTemp |= (uint16_t) DMA_CTS_TCNT_LKEN_TCNT(count);  /* PRQA S 2985 */
        base->CTS[channel].TCNT.LKEN = regValTemp;
    } else
    {
        regValTemp = base->CTS[channel].TCNTRV;
        regValTemp &= (uint16_t) ~(DMA_CTS_TCNT_LKDIS_TCNT_MASK);
        regValTemp |= (uint16_t) DMA_CTS_TCNT_LKDIS_TCNT(count);  /* PRQA S 2985 */
        base->CTS[channel].TCNTRV = regValTemp;

        regValTemp = base->CTS[channel].TCNT.LKDIS;
        regValTemp &= (uint16_t) ~(DMA_CTS_TCNT_LKDIS_TCNT_MASK);
        regValTemp |= (uint16_t) DMA_CTS_TCNT_LKDIS_TCNT(count);  /* PRQA S 2985 */
        base->CTS[channel].TCNT.LKDIS = regValTemp;
    }
}

/*!
 * @brief Gets the current trigger loop iteration count according to transfer loop channel link setting.
 */
uint32_t DMA_CTSGetCurrentTriggerCount(const DMA_Type *base, uint8_t channel)
{
#if defined (CUSTOM_DEVASSERT) || defined (DEV_ERROR_DETECT)
    DEV_ASSERT(channel < FEATURE_DMA_CHANNELS);
#endif
    uint16_t result;
    if ((base->CTS[channel].TCNTRV & DMA_CTS_TCNT_LKDIS_LKEN_MASK) == DMA_CTS_TCNT_LKDIS_LKEN_MASK)
    {
        result = (uint16_t) ((base->CTS[channel].TCNT.LKEN & DMA_CTS_TCNT_LKEN_TCNT_MASK)
            >> DMA_CTS_TCNT_LKEN_TCNT_SHIFT);
    } else
    {
        result = (uint16_t) ((base->CTS[channel].TCNT.LKDIS & DMA_CTS_TCNT_LKDIS_TCNT_MASK)
            >> DMA_CTS_TCNT_LKDIS_TCNT_SHIFT);
    }
    return (uint32_t) result;
}

#ifdef FEATURE_DMAMUX_AVAILABLE

/*!
 * @brief Initialize the dmamux module to the reset state.
 */
void DMAMUX_Init(DMA_Type *base)
{
    uint8_t i;

    for (i = 0; i < FEATURE_DMA_CHANNELS; i++)
    {
        base->CHMUX[i] = 0;
    }
}

#endif

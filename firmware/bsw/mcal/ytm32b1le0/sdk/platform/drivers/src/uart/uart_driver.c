/*
 * Copyright 2020-2022 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * @file uart_driver.c
 * @brief UART driver implementation - public API and internal helpers.
 *
 * Contains initialization, data transfer (interrupt / DMA / polling),
 * baud-rate configuration, callback management, modem control,
 * one-wire mode, and the common IRQ dispatcher.
 */

/*!
 * @page misra_violations MISRA-C:2012 violations list
 *
 * PRQA S 0326 Rule 11.6: Cast between a pointer to void and an integral type.
 *
 * PRQA S 3387 Rule 13.3: A full expression containing an increment (++) or 
 *                        decrement (--) operator should have no potential side effects 
 *                        other than that caused by the increment or decrement operator.
 */

#include <stddef.h>
#include <stdbool.h>
#include "uart_hw_access.h"
#include "uart_irq.h"
#include "clock_manager.h"
#include <stdint.h>

/******************************************************************************
* Define
*******************************************************************************/

#define UART_DEFAULT_TX_FIFO_WATERMARK (0x02)
/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Pointer to uart runtime state structure */
static uart_state_t *s_uartStatePtr[UART_INSTANCE_COUNT] = FEATURE_UART_STATE_PTR;

/* Table of base addresses for uart instances. */
static UART_Type *const s_uartBase[UART_INSTANCE_COUNT] = UART_BASE_PTRS;

/* Table to save UART enum numbers defined in CMSIS files. */
static const IRQn_Type s_uartRxTxIrqId[UART_INSTANCE_COUNT] = UART_RX_TX_IRQS;

/* Table to save UART clock names as defined in clock manager. */
static const clock_names_t s_uartClkNames[UART_INSTANCE_COUNT] = UART_CLOCK_NAMES;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
static status_t UART_DRV_StartSendDataUsingInt(uint32_t instance, const uint8_t *txBuff, uint32_t txSize);
static void UART_DRV_CompleteSendDataUsingInt(uint32_t instance);
static status_t UART_DRV_StartReceiveDataUsingInt(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize);
static void UART_DRV_CompleteReceiveDataUsingInt(uint32_t instance);
#if FEATURE_UART_HAS_DMA_ENABLE
static void UART_DRV_StopTxDma(uint32_t instance);
static void UART_DRV_StopRxDma(uint32_t instance);
static status_t UART_DRV_StartSendDataUsingDma(uint32_t instance, const uint8_t *txBuff, uint32_t txSize);
static void UART_DRV_TxDmaCallback(void *parameter, dma_chn_status_t status);
static status_t UART_DRV_StartReceiveDataUsingDma(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize);
static void UART_DRV_RxDmaCallback(void *parameter, dma_chn_status_t status);
#endif
static void UART_DRV_PutData(uint32_t instance);
static void UART_DRV_GetData(uint32_t instance);
static void UART_DRV_RxIrqHandler(uint32_t instance);
static void UART_DRV_TxEmptyIrqHandler(uint32_t instance);
static void UART_DRV_TxCompleteIrqHandler(uint32_t instance);
static void UART_DRV_ErrIrqHandler(uint32_t instance);
static void UART_DRV_SetErrorInterrupts(uint32_t instance, bool enable);

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Fill a configuration structure with safe default values.
 *
 * Defaults: 9600 baud, 8N1, interrupt-based transfer.
 *
 * @param[out] uartUserConfig  Pointer to the configuration structure to populate.
 */
void UART_DRV_GetDefaultConfig(uart_user_config_t *uartUserConfig)
{
    DEV_ASSERT(uartUserConfig != NULL);

    uartUserConfig->transferType = UART_USING_INTERRUPTS;
    uartUserConfig->baudRate = 9600U;
    uartUserConfig->parityMode = UART_PARITY_DISABLED;
    uartUserConfig->stopBitCount = UART_ONE_STOP_BIT;
    uartUserConfig->bitCountPerChar = UART_8_BITS_PER_CHAR;
    uartUserConfig->rxDMAChannel = 0U;
    uartUserConfig->txDMAChannel = 0U;
}

/*!
 * @brief Initialize a UART instance for operation.
 *
 * Resets the peripheral, configures baud rate / frame format / idle detection,
 * creates synchronisation semaphores, installs the ISR, and enables the NVIC
 * interrupt.  The caller provides memory for the runtime state structure.
 *
 * @param[in]  instance        UART instance index (0-based).
 * @param[out] uartStatePtr    Pointer to caller-allocated runtime state.
 * @param[in]  uartUserConfig  Pointer to the user configuration structure.
 * @return STATUS_SUCCESS on success; STATUS_ERROR if semaphore creation fails.
 */
status_t UART_DRV_Init(uint32_t instance, uart_state_t *uartStatePtr, const uart_user_config_t *uartUserConfig)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(uartStatePtr != NULL);
    DEV_ASSERT(uartUserConfig != NULL);

    status_t osStatusRxSem;
    status_t osStatusTxSem;
    status_t retVal = STATUS_SUCCESS;

    UART_Type *base = s_uartBase[instance];
    uint32_t idx;
    uint32_t uartSourceClock;
    clock_names_t instanceClkName = s_uartClkNames[instance];

    /* Get the UART clock as configured in the clock manager */
    (void)CLOCK_SYS_GetFreq(instanceClkName, &uartSourceClock);

    /* Check if current instance is clock gated off. */
    DEV_ASSERT(uartSourceClock > 0U);

    /* Check if current instance is already initialized. */
    DEV_ASSERT(s_uartStatePtr[instance] == NULL);

#if FEATURE_UART_HAS_DMA_ENABLE
    /* In DMA mode, only 8-bits chars are supported */
    DEV_ASSERT((uartUserConfig->transferType != UART_USING_DMA) ||
               (uartUserConfig->bitCountPerChar <= UART_8_BITS_PER_CHAR));
#endif

    /* For 10 bits per char, parity bit cannot be enabled */
    DEV_ASSERT((uartUserConfig->bitCountPerChar != UART_10_BITS_PER_CHAR) ||
               (uartUserConfig->parityMode == UART_PARITY_DISABLED));

    /* Clear the state struct for this instance. */
    uint8_t *clearStructPtr = (uint8_t *)uartStatePtr;
    for (idx = 0; idx < sizeof(uart_state_t); idx++)
    {
        clearStructPtr[idx] = 0;
    }

    /* Save runtime structure pointer.*/
    s_uartStatePtr[instance] = uartStatePtr;

    /* Save whether to enable idle interrupt. */
    uartStatePtr->idleErrorIntEnable = uartUserConfig->idleErrorIntEnable;

    /* Save the transfer information for runtime retrieval */
    uartStatePtr->transferType = uartUserConfig->transferType;
    uartStatePtr->bitCountPerChar = uartUserConfig->bitCountPerChar;
#if FEATURE_UART_HAS_DMA_ENABLE
    uartStatePtr->rxDMAChannel = uartUserConfig->rxDMAChannel;
    uartStatePtr->txDMAChannel = uartUserConfig->txDMAChannel;
#endif

    /* IPC software reset: restore all UART registers to hardware defaults */
    CLOCK_DRV_ResetModule(s_uartClkNames[instance]);

    /* initialize the parameters of the UART config structure with desired data */
    (void)UART_DRV_SetBaudRate(instance, uartUserConfig->baudRate);

    if (uartUserConfig->parityMode != UART_PARITY_DISABLED)
    {
        UART_SetBitCountPerChar(base, uartUserConfig->bitCountPerChar, true);
    }
    else
    {
        UART_SetBitCountPerChar(base, uartUserConfig->bitCountPerChar, false);
    }
    UART_SetParityMode(base, uartUserConfig->parityMode);
    UART_SetStopBitCount(base, uartUserConfig->stopBitCount);
    (void)UART_DRV_SetLineIdleDetect(instance, UART_LINE_IDLE_SIZE_2_CHAR, false);

    /* initialize last driver operation status */
    uartStatePtr->transmitStatus = STATUS_SUCCESS;
    uartStatePtr->receiveStatus = STATUS_SUCCESS;

    /* Create the synchronization objects */
    osStatusRxSem = OSIF_SemaCreate(&uartStatePtr->rxComplete, 0);
    osStatusTxSem = OSIF_SemaCreate(&uartStatePtr->txComplete, 0);
    if ((osStatusRxSem == STATUS_ERROR) || (osStatusTxSem == STATUS_ERROR))
    {
        retVal = STATUS_ERROR;
    }
    
    if (retVal != STATUS_ERROR)
    {
        /* Install UART irq handler */
        INT_SYS_InstallHandler(s_uartRxTxIrqId[instance], g_uartIsr[instance], (isr_t *)0);

        /* Enable UART interrupt. */
        INT_SYS_EnableIRQ(s_uartRxTxIrqId[instance]);  
    }

    return retVal;
}

/*!
 * @brief De-initialize a UART instance.
 *
 * Waits for any in-progress transmission to finish, resets the peripheral,
 * destroys semaphores, disables the NVIC interrupt, and restores the default
 * ISR handler.
 *
 * @param[in] instance  UART instance index (0-based).
 * @return STATUS_SUCCESS always.
 */
status_t UART_DRV_Deinit(uint32_t instance)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    clock_names_t instanceClkName = s_uartClkNames[instance];
    uint32_t uartSourceClock;
    const UART_Type *base = s_uartBase[instance];
    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    (void)CLOCK_SYS_GetFreq(instanceClkName, &uartSourceClock);

    /* Check if current instance is already de-initialized or is gated.*/
    DEV_ASSERT(s_uartStatePtr[instance] != NULL);
    DEV_ASSERT(uartSourceClock > 0U);

    /* Wait until the data is completely shifted out of shift register */
    while (!UART_GetStatusFlag(base, UART_TX_COMPLETE))
    {
    }

    /* ipc software reset */
    CLOCK_DRV_ResetModule(s_uartClkNames[instance]);

    /* Destroy the synchronization objects */
    (void)OSIF_SemaDestroy(&uartState->rxComplete);
    (void)OSIF_SemaDestroy(&uartState->txComplete);

    /* Disable UART interrupt. */
    INT_SYS_DisableIRQ(s_uartRxTxIrqId[instance]);

    /* Restore default handler. */
    INT_SYS_InstallHandler(s_uartRxTxIrqId[instance], DefaultISR, (isr_t *)0);

    /* Clear our saved pointer to the state structure */
    s_uartStatePtr[instance] = NULL;

    return STATUS_SUCCESS;
}

/*!
 * @brief Install or replace the receive callback function.
 *
 * @param[in] instance       UART instance index (0-based).
 * @param[in] function       New callback (NULL to uninstall).
 * @param[in] callbackParam  User-defined parameter forwarded to the callback.
 * @return The previously installed callback pointer.
 */
uart_callback_t UART_DRV_InstallRxCallback(uint32_t instance, uart_callback_t function, void *callbackParam)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    uart_callback_t currentCallback = uartState->rxCallback;
    uartState->rxCallback = function;
    uartState->rxCallbackParam = callbackParam;

    return currentCallback;
}

/*!
 * @brief Install or replace the transmit callback function.
 *
 * @param[in] instance       UART instance index (0-based).
 * @param[in] function       New callback (NULL to uninstall).
 * @param[in] callbackParam  User-defined parameter forwarded to the callback.
 * @return The previously installed callback pointer.
 */
uart_callback_t UART_DRV_InstallTxCallback(uint32_t instance, uart_callback_t function, void *callbackParam)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    uart_callback_t currentCallback = uartState->txCallback;
    uartState->txCallback = function;
    uartState->txCallbackParam = callbackParam;

    return currentCallback;
}

/*!
 * @brief Send data using a blocking (synchronous) method.
 *
 * Starts an interrupt-based or DMA-based transmission and blocks until all
 * bytes have been sent or the timeout expires.
 *
 * @param[in] instance  UART instance index (0-based).
 * @param[in] txBuff    Pointer to the transmit data buffer.
 * @param[in] txSize    Number of bytes to transmit.
 * @param[in] timeout   Timeout in milliseconds.
 * @return STATUS_SUCCESS, STATUS_BUSY, STATUS_TIMEOUT, or STATUS_ERROR.
 */
status_t UART_DRV_SendDataBlocking(uint32_t instance, const uint8_t *txBuff, uint32_t txSize, uint32_t timeout)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retVal = STATUS_SUCCESS;
    status_t syncStatus;

    /* Indicates this is a blocking transaction. */
    uartState->isTxBlocking = true;
    /* Reset signal before transfer */
    (void)OSIF_SemaWait(&(uartState->txComplete), 0);

    DEV_ASSERT((uartState->transferType == UART_USING_INTERRUPTS) || (uartState->transferType == UART_USING_DMA));

    if (uartState->transferType == UART_USING_INTERRUPTS)
    {
        /* Start the transmission process using interrupts */
        retVal = UART_DRV_StartSendDataUsingInt(instance, txBuff, txSize);
    }
#if FEATURE_UART_HAS_DMA_ENABLE
    else
    {
        /* Start the transmission process using DMA */
        retVal = UART_DRV_StartSendDataUsingDma(instance, txBuff, txSize);
    }
#endif

    if (retVal == STATUS_SUCCESS)
    {
        /* Wait until the transmit is complete. */
        syncStatus = OSIF_SemaWait(&uartState->txComplete, timeout);

        /* Finish the transmission if timeout expired */
        if (syncStatus == STATUS_TIMEOUT)
        {
            uartState->isTxBlocking = false;
            uartState->transmitStatus = STATUS_TIMEOUT;

            if (uartState->transferType == UART_USING_INTERRUPTS)
            {
                UART_DRV_CompleteSendDataUsingInt(instance);
            }
#if FEATURE_UART_HAS_DMA_ENABLE
            else
            {
                UART_DRV_StopTxDma(instance);
            }
#endif
        }
    }

    return uartState->transmitStatus;
}

/*!
 * @brief Send data by polling the TX-empty flag (no interrupts / DMA).
 *
 * Blocks until all bytes have been written to the DATA register.
 *
 * @param[in] instance  UART instance index (0-based).
 * @param[in] txBuff    Pointer to the transmit data buffer.
 * @param[in] txSize    Number of bytes to transmit.
 * @return STATUS_SUCCESS or STATUS_BUSY.
 */
status_t UART_DRV_SendDataPolling(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    const uint8_t *currentTxBuff = txBuff; 
    uint32_t remainingSize = txSize; 
    status_t retVal = STATUS_SUCCESS;

    /* Check the validity of the parameters */
    DEV_ASSERT(txSize > 0U);
    DEV_ASSERT((uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR) || ((txSize & 1U) == 0U));

    /* Check driver is not busy transmitting data from a previous asynchronous call */
    if (uartState->isTxBusy)
    {
        retVal = STATUS_BUSY;
    }

    if (retVal == STATUS_SUCCESS)
    {
        /* Enable the UART transmitter */
        UART_SetTransmitterCmd(base, true);

        while (remainingSize > 0U)
        {
            while (!UART_GetStatusFlag(base, UART_TX_DATA_REG_EMPTY))
            {
            }

            uartState->txBuff = currentTxBuff;
            UART_DRV_PutData(instance);

            if (uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR)
            {
                ++currentTxBuff;
                --remainingSize;
            }
            else
            {
                ++currentTxBuff;
                ++currentTxBuff;
                remainingSize -= 2U;
            }
        }
        /* Wait Transfer complete done */
        while (!UART_GetStatusFlag(base, UART_TX_COMPLETE))
        {
        }
        /* Disable the UART transmitter */
        UART_SetTransmitterCmd(base, false);
    }
    else
    {

    }

    return retVal;
}

/*!
 * @brief Start a non-blocking (asynchronous) transmit.
 *
 * Returns immediately. Use UART_DRV_GetTransmitStatus() to query progress.
 *
 * @param[in] instance  UART instance index (0-based).
 * @param[in] txBuff    Pointer to the transmit data buffer.
 * @param[in] txSize    Number of bytes to transmit.
 * @return STATUS_SUCCESS or STATUS_BUSY.
 */
status_t UART_DRV_SendData(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);

    status_t retVal = STATUS_SUCCESS;
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Indicates this is a non-blocking transaction. */
    uartState->isTxBlocking = false;

    DEV_ASSERT((uartState->transferType == UART_USING_INTERRUPTS) || (uartState->transferType == UART_USING_DMA));

    if (uartState->transferType == UART_USING_INTERRUPTS)
    {
        /* Start the transmission process using interrupts */
        retVal = UART_DRV_StartSendDataUsingInt(instance, txBuff, txSize);
    }
#if FEATURE_UART_HAS_DMA_ENABLE
    else
    {
        /* Start the transmission process using DMA */
        retVal = UART_DRV_StartSendDataUsingDma(instance, txBuff, txSize);
    }
#endif

    return retVal;
}

/*!
 * @brief Query the status of an ongoing non-blocking transmit.
 *
 * @param[in]  instance        UART instance index (0-based).
 * @param[out] bytesRemaining  Remaining bytes still to send (may be NULL).
 * @return STATUS_SUCCESS, STATUS_BUSY, STATUS_UART_ABORTED, STATUS_TIMEOUT,
 *         or STATUS_ERROR.
 */
status_t UART_DRV_GetTransmitStatus(uint32_t instance, uint32_t *bytesRemaining)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    if (bytesRemaining != NULL)
    {
        if (uartState->isTxBusy)
        {
            /* Fill in the bytes not transferred yet. */
            if (uartState->transferType == UART_USING_INTERRUPTS)
            {
                /* In interrupt-based communication, the remaining bytes are retrieved
                 * from the state structure
                 */
                *bytesRemaining = uartState->txSize;
                ;
            }
#if FEATURE_UART_HAS_DMA_ENABLE
            else
            {
                /* In DMA-based communication, the remaining bytes are retrieved
                 * from the current DMA trigger loop count
                 */
                *bytesRemaining = DMA_DRV_GetRemainingTriggerIterationsCount(uartState->txDMAChannel);
            }
#endif
        }
        else
        {
            *bytesRemaining = 0;
        }
    }

    return uartState->transmitStatus;
}

/*!
 * @brief Abort an ongoing non-blocking transmit.
 *
 * Stops the current transmission and sets the transmit status to
 * STATUS_UART_ABORTED.
 *
 * @param[in] instance  UART instance index (0-based).
 * @return STATUS_SUCCESS always.
 */
status_t UART_DRV_AbortSendingData(uint32_t instance)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    status_t retVal = STATUS_BUSY;
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Check if a transfer is running. */
    if (!uartState->isTxBusy)
    {
        retVal = STATUS_SUCCESS;
        
    }
    else
    {
        /* Uart state is sending */
    }
    
    if (retVal != STATUS_SUCCESS)
    {    /* Update the tx status */
        uartState->transmitStatus = STATUS_UART_ABORTED;

        /* Stop the running transfer. */
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            UART_DRV_CompleteSendDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            UART_DRV_StopTxDma(instance);
        }
#endif
        retVal = STATUS_SUCCESS;
    }
    else
    {
        /* Uart state is not busy */
    }
    
    return retVal;
}

/*!
 * @brief Receive data using a blocking (synchronous) method.
 *
 * Starts an interrupt-based or DMA-based reception and blocks until all
 * bytes have been received or the timeout expires.
 *
 * @param[in]  instance  UART instance index (0-based).
 * @param[out] rxBuff    Pointer to the receive data buffer.
 * @param[in]  rxSize    Number of bytes to receive.
 * @param[in]  timeout   Timeout in milliseconds.
 * @return STATUS_SUCCESS, STATUS_BUSY, STATUS_TIMEOUT, or an error code.
 */
status_t UART_DRV_ReceiveDataBlocking(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize, uint32_t timeout)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retVal = STATUS_SUCCESS;
    status_t syncStatus;

    /* Indicates this is a blocking transaction. */
    uartState->isRxBlocking = true;
    /* Reset signal before transfer */
    (void)OSIF_SemaWait(&(uartState->rxComplete), 0);

    DEV_ASSERT((uartState->transferType == UART_USING_INTERRUPTS) || (uartState->transferType == UART_USING_DMA));

    if (uartState->transferType == UART_USING_INTERRUPTS)
    {
        /* Start the reception process using interrupts */
        retVal = UART_DRV_StartReceiveDataUsingInt(instance, rxBuff, rxSize);
    }
#if FEATURE_UART_HAS_DMA_ENABLE
    else
    {
        /* Start the reception process using DMA */
        retVal = UART_DRV_StartReceiveDataUsingDma(instance, rxBuff, rxSize);
    }
#endif

    if (retVal == STATUS_SUCCESS)
    {
        /* Wait until the receive is complete. */
        syncStatus = OSIF_SemaWait(&uartState->rxComplete, timeout);

        /* Finish the reception if timeout expired */
        if (syncStatus == STATUS_TIMEOUT)
        {
            uartState->isRxBlocking = false;
            uartState->receiveStatus = STATUS_TIMEOUT;

            if (uartState->transferType == UART_USING_INTERRUPTS)
            {
                UART_DRV_CompleteReceiveDataUsingInt(instance);
            }
#if FEATURE_UART_HAS_DMA_ENABLE
            else
            {
                UART_DRV_StopRxDma(instance);
            }
#endif
        }
    }

    return uartState->receiveStatus;
}

/*!
 * @brief Receive data by polling the RX-full flag (no interrupts / DMA).
 *
 * Blocks until all bytes have been read or an error is detected.
 *
 * @param[in]  instance  UART instance index (0-based).
 * @param[out] rxBuff    Pointer to the receive data buffer.
 * @param[in]  rxSize    Number of bytes to receive.
 * @return STATUS_SUCCESS, STATUS_BUSY, or STATUS_UART_xxx error.
 */
status_t UART_DRV_ReceiveDataPolling(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);

    status_t retVal = STATUS_SUCCESS;
    status_t tmpState = STATUS_SUCCESS;
    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    bool statusFlag = false;

    uint8_t *currentRxBuff = rxBuff; 
    uint32_t remainingSize = rxSize; 
    /* Check the validity of the parameters */
    DEV_ASSERT(rxSize > 0U);
    DEV_ASSERT((uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR) || ((rxSize & 1U) == 0U));

    /* Check driver is not busy receiving data from a previous asynchronous call */
    if (uartState->isRxBusy)
    {
        retVal = STATUS_BUSY;
    }

    if (retVal == STATUS_SUCCESS)
    {
         /* Enable the UART receiver */
        UART_SetReceiverCmd((UART_Type *)base, true);

#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
        /* Initialize rx fifo */
        UART_ResetRxFifo(base);
        UART_SetRxFifoWatermark(base, 0);
        UART_EnableRxFifo(base, true);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

        while (remainingSize > 0U)
        {
            while (!UART_GetStatusFlag(base, UART_RX_DATA_REG_FULL))
            {
            }

            uartState->rxBuff = currentRxBuff;
            UART_DRV_GetData(instance);

            if (uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR)
            {
                ++currentRxBuff;
                --remainingSize;
            }
            else
            {
                ++currentRxBuff;
                ++currentRxBuff;
                remainingSize -= 2U;
            }

            /* Check for errors on received data */
            if (UART_GetStatusFlag(base, UART_FRAME_ERR))
            {
                tmpState = STATUS_UART_FRAMING_ERROR;
                /* Disable the UART receiver */
                UART_SetReceiverCmd((UART_Type *)base, false);
                /* Clear the flag */
                (void)UART_ClearStatusFlag(base, UART_FRAME_ERR);
                statusFlag = true;
            }
            else if (UART_GetStatusFlag(base, UART_NOISE_DETECT))
            {
                tmpState = STATUS_UART_NOISE_ERROR;
                /* Disable the UART receiver */
                UART_SetReceiverCmd((UART_Type *)base, false);
                /* Clear the flag */
                (void)UART_ClearStatusFlag(base, UART_NOISE_DETECT);
                statusFlag = true;
            }
            else if (UART_GetStatusFlag(base, UART_PARITY_ERR))
            {
                tmpState = STATUS_UART_PARITY_ERROR;
                /* Disable the UART receiver */
                UART_SetReceiverCmd((UART_Type *)base, false);
                /* Clear the flag */
                (void)UART_ClearStatusFlag(base, UART_PARITY_ERR);
                statusFlag = true;
            }
            else if (UART_GetStatusFlag(base, UART_RX_OVERRUN))
            {
                tmpState = STATUS_UART_RX_OVERRUN;
                /* Disable the UART receiver */
                UART_SetReceiverCmd((UART_Type *)base, false);
                /* Clear the flag */
                (void)UART_ClearStatusFlag(base, UART_RX_OVERRUN);
                statusFlag = true;
            }
            else
            {
                statusFlag = false;
            }

            if (statusFlag)
            {
                break;
            }
        }

        /* Update received status */
        if ((remainingSize == 0U) && (tmpState == STATUS_UART_RX_OVERRUN))
        {
            retVal = STATUS_SUCCESS;
        }
        else
        {
            retVal = tmpState;
        }

        if (retVal == STATUS_SUCCESS)
        {
            /* Disable the UART receiver */
            UART_SetReceiverCmd((UART_Type *)base, false);
        }
#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
        /* Reset rx fifo to clear RDRF flag */
        UART_ResetRxFifo(base);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */
    }

    return retVal;
}

/*!
 * @brief Start a non-blocking (asynchronous) receive.
 *
 * Returns immediately. Use UART_DRV_GetReceiveStatus() to query progress.
 *
 * @param[in]  instance  UART instance index (0-based).
 * @param[out] rxBuff    Pointer to the receive data buffer.
 * @param[in]  rxSize    Number of bytes to receive.
 * @return STATUS_SUCCESS or STATUS_BUSY.
 */
status_t UART_DRV_ReceiveData(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);

    status_t retVal = STATUS_SUCCESS;
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Indicates this is a non-blocking transaction. */
    uartState->isRxBlocking = false;

    DEV_ASSERT((uartState->transferType == UART_USING_INTERRUPTS) || (uartState->transferType == UART_USING_DMA));

    if (uartState->transferType == UART_USING_INTERRUPTS)
    {
        /* Start the reception process using interrupts */
        retVal = UART_DRV_StartReceiveDataUsingInt(instance, rxBuff, rxSize);
    }
#if FEATURE_UART_HAS_DMA_ENABLE
    else
    {
        /* Start the reception process using DMA */
        retVal = UART_DRV_StartReceiveDataUsingDma(instance, rxBuff, rxSize);
    }
#endif

    return retVal;
}

/*!
 * @brief Query the status of an ongoing non-blocking receive.
 *
 * @param[in]  instance        UART instance index (0-based).
 * @param[out] bytesRemaining  Remaining bytes still to receive (may be NULL).
 * @return STATUS_SUCCESS, STATUS_BUSY, STATUS_UART_ABORTED, STATUS_TIMEOUT,
 *         or an error code.
 */
status_t UART_DRV_GetReceiveStatus(uint32_t instance, uint32_t *bytesRemaining)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    if (bytesRemaining != NULL)
    {
        if (uartState->isRxBusy)
        {
            /* Fill in the bytes transferred. */
            if (uartState->transferType == UART_USING_INTERRUPTS)
            {
                /* In interrupt-based communication, the remaining bytes are retrieved
                 * from the state structure
                 */
                *bytesRemaining = uartState->rxSize;
            }
#if FEATURE_UART_HAS_DMA_ENABLE
            else
            {
                /* In DMA-based communication, the remaining bytes are retrieved
                 * from the current DMA trigger loop count
                 */
                *bytesRemaining = DMA_DRV_GetRemainingTriggerIterationsCount(uartState->rxDMAChannel);
            }
#endif
        }
        else
        {
            *bytesRemaining = 0;
        }
    }

    return uartState->receiveStatus;
}

/*!
 * @brief Abort an ongoing non-blocking receive.
 *
 * Stops the current reception and sets the receive status to
 * STATUS_UART_ABORTED.
 *
 * @param[in] instance  UART instance index (0-based).
 * @return STATUS_SUCCESS always.
 */
status_t UART_DRV_AbortReceivingData(uint32_t instance)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Check if a transfer is running. */
    if (uartState->isRxBusy)
    {
        /* Update the rx status */
        uartState->receiveStatus = STATUS_UART_ABORTED;

        /* Stop the running transfer. */
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            UART_DRV_CompleteReceiveDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            UART_DRV_StopRxDma(instance);
        }
#endif
    }

    return STATUS_SUCCESS;
}

/*!
 * @brief Reconfigure the baud rate at runtime.
 *
 * Automatically selects the best oversampling ratio (OSR) for the given
 * clock and desired baud rate.  Enables both-edge sampling when OSR < 8.
 *
 * @param[in] instance         UART instance index (0-based).
 * @param[in] desiredBaudRate  Target baud rate in bps.
 * @return STATUS_SUCCESS or STATUS_BUSY if a transfer is in progress.
 */
status_t UART_DRV_SetBaudRate(uint32_t instance, uint32_t desiredBaudRate)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    uint16_t sbr, sbrTemp, i;
    uint32_t osr, tempDiff, calculatedBaud, baudDiff, maxOsr;
    uint32_t uartSourceClock;
    clock_names_t instanceClkName = s_uartClkNames[instance];
    UART_Type *base = s_uartBase[instance];
    const uart_state_t *uartState;
    uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retVal = STATUS_SUCCESS;

    if (uartState != NULL)
    {
        /* Check if there is an ongoing transfer */
        if (uartState->isTxBusy == true)
        {
            retVal = STATUS_BUSY;
        }
        else if (uartState->isRxBusy == true)
        {
            retVal = STATUS_BUSY;
        }
        else
        {
            /*Nothing to do*/
        }
    }

    if (retVal != STATUS_BUSY)
    {
        /* Get the UART clock as configured in the clock manager */
        (void)CLOCK_SYS_GetFreq(instanceClkName, &uartSourceClock);

        /* Check if current instance is clock gated off. */
        DEV_ASSERT(uartSourceClock > 0U);
        /* Check if the desired baud rate can be configured with the current protocol clock. */
        DEV_ASSERT(uartSourceClock >= (desiredBaudRate * 4U));

        /* This uart instantiation uses a slightly different baud rate calculation
        * The idea is to use the best OSR (over-sampling rate) possible
        * Note, osr is typically hard-set to 16 in other uart instantiations
        * First calculate the baud rate using the minimum OSR possible (4) */
        osr = 4;
        sbr = (uint16_t)(uartSourceClock / (desiredBaudRate * osr));
        calculatedBaud = (uartSourceClock / (osr * sbr));
        if (calculatedBaud > desiredBaudRate)
        {
            baudDiff = calculatedBaud - desiredBaudRate;
        }
        else
        {
            baudDiff = desiredBaudRate - calculatedBaud;
        }
        /* find maximum osr */
        maxOsr = uartSourceClock / desiredBaudRate;
        if (maxOsr > 32U)
        {
            maxOsr = 32U;
        }
        /* loop to find the best osr value possible, one that generates minimum baudDiff
        * iterate through the rest of the supported values of osr */
        if (maxOsr >= 5U)
        {
            for (i = 5U; i <= maxOsr; i++)
            {
                /* calculate the temporary sbr value   */
                sbrTemp = (uint16_t)(uartSourceClock / (desiredBaudRate * i));
                /* calculate the baud rate based on the temporary osr and sbr values */
                calculatedBaud = (uartSourceClock / (i * (uint32_t)sbrTemp));

                if (calculatedBaud > desiredBaudRate)
                {
                    tempDiff = calculatedBaud - desiredBaudRate;
                }
                else
                {
                    tempDiff = desiredBaudRate - calculatedBaud;
                }

                if (tempDiff <= baudDiff)
                {
                    baudDiff = tempDiff;
                    osr = i;       /* update and store the best osr value calculated */
                    sbr = sbrTemp; /* update store the best sbr value calculated */
                }
            }
        }
        /* Check if osr is between 4x and 7x oversampling.
        * If so, then "BOTHEDGE" sampling must be turned on */
        if (osr < 8U)
        {
            UART_EnableBothEdgeSamplingCmd(base);
        }

        /* program the osr value (bit value is one less than actual value) */
        UART_SetOversamplingRatio(base, (osr - 1U));

        /* write the sbr value to the BAUD registers */
        UART_SetBaudRateDivisor(base, sbr);
    }

    return retVal;
}

/*!
 * @brief Read back the currently configured baud rate.
 *
 * @param[in]  instance            UART instance index (0-based).
 * @param[out] configuredBaudRate  Pointer to store the baud rate value.
 */
void UART_DRV_GetBaudRate(uint32_t instance, uint32_t *configuredBaudRate)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(configuredBaudRate != NULL);

    uint8_t osr;
    uint16_t sbr;
    uint32_t uartSourceClock;
    clock_names_t instanceClkName = s_uartClkNames[instance];
    const UART_Type *base = s_uartBase[instance];

    /* Get the UART clock as configured in the clock manager */
    (void)CLOCK_SYS_GetFreq(instanceClkName, &uartSourceClock);

    osr = UART_GetOversamplingRatio(base);
    sbr = UART_GetBaudRateDivisor(base);

    *configuredBaudRate = (uartSourceClock / ((osr + 1UL) * sbr));
}

/*!
 * @brief Provide a new transmit buffer for continuous (streaming) transmission.
 *
 * Typically called from the TX callback to chain the next data block.
 *
 * @param[in] instance  UART instance index (0-based).
 * @param[in] txBuff    Pointer to the new transmit buffer.
 * @param[in] txSize    Size of the new buffer in bytes.
 * @return STATUS_SUCCESS always.
 */
status_t UART_DRV_SetTxBuffer(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);
    DEV_ASSERT(txSize > 0U);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    uartState->txBuff = txBuff;
    uartState->txSize = txSize;

    return STATUS_SUCCESS;
}

/*!
 * @brief Provide a new receive buffer for continuous (streaming) reception.
 *
 * Typically called from the RX callback to chain the next data block.
 *
 * @param[in]  instance  UART instance index (0-based).
 * @param[out] rxBuff    Pointer to the new receive buffer.
 * @param[in]  rxSize    Size of the new buffer in bytes.
 * @return STATUS_SUCCESS always.
 */
status_t UART_DRV_SetRxBuffer(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);
    DEV_ASSERT(rxSize > 0U);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    uartState->rxBuff = rxBuff;
    uartState->rxSize = rxSize;

    return STATUS_SUCCESS;
}

/*!
 * @brief Configure idle-line detection length and interrupt enable.
 *
 * @param[in] instance   UART instance index (0-based).
 * @param[in] idleSize   Idle-line length in character-times.
 * @param[in] intEnable  true = enable idle-line interrupt, false = disable.
 * @return STATUS_SUCCESS always.
 */
status_t UART_DRV_SetLineIdleDetect(uint32_t instance, uart_line_size_t idleSize, bool intEnable)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    UART_Type *base = s_uartBase[instance];
    UART_SetLineIdleSize(base, (uint8_t)idleSize);
    UART_SetIntMode(base, UART_INT_IDLE_LINE, intEnable);
    return STATUS_SUCCESS;
}

/*!
 * @brief Common UART interrupt handler.
 *
 * Dispatches to the appropriate sub-handler (RX, TX-empty, TX-complete,
 * error) based on interrupt flags.  Called from per-instance ISR wrappers.
 *
 * @param[in] instance  UART instance index (0-based).
 */
void UART_DRV_IRQHandler(uint32_t instance)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    const UART_Type *base = s_uartBase[instance];

    /* Handle receive data full interrupt */
    if (UART_GetIntMode(base, UART_INT_RX_DATA_REG_FULL))
    {
        if (UART_GetStatusFlag(base, UART_RX_DATA_REG_FULL))
        {
            UART_DRV_RxIrqHandler(instance);
        }
    }

    /* Handle error interrupt */
    UART_DRV_ErrIrqHandler(instance);

    /* Handle transmitter data register empty interrupt */
    if (UART_GetIntMode(base, UART_INT_TX_DATA_REG_EMPTY))
    {
        if (UART_GetStatusFlag(base, UART_TX_DATA_REG_EMPTY))
        {
            UART_DRV_TxEmptyIrqHandler(instance);
        }
    }

    /* Handle transmission complete interrupt */
    if (UART_GetIntMode(base, UART_INT_TX_COMPLETE))
    {
        if (UART_GetStatusFlag(base, UART_TX_COMPLETE))
        {
            UART_DRV_TxCompleteIrqHandler(instance);
        }
    }
}

/*!
 * @brief RX data-ready interrupt sub-handler.
 *
 * Reads received data from the FIFO (or DATA register), updates the state
 * buffer pointer, and invokes the user callback when the buffer is full.
 *
 * @param[in] instance  UART instance index (0-based).
 */
static void UART_DRV_RxIrqHandler(uint32_t instance)
{
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    uint32_t rxFifoWordCount;

#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
    UART_Type *base = s_uartBase[instance];
    rxFifoWordCount = UART_GetWordCountInRxFifo(base);
#else
    rxFifoWordCount = 1;
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

    for (uint32_t i = 0; i < rxFifoWordCount; i++)
    {
        /* Get data and put in receive buffer  */
        UART_DRV_GetData(instance);

        /* Update the internal state */
        if (uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR)
        {
            ++uartState->rxBuff;
            --uartState->rxSize; /* PRQA S 3387 */
        }
        else
        {
            uartState->rxBuff = &uartState->rxBuff[2];
            uartState->rxSize -= 2U;
        }

        /* Check if this was the last byte in the current buffer */
        if (uartState->rxSize == 0U)
        {
            /* Invoke callback if there is one (callback may reset the rx buffer for continuous reception) */
            if (uartState->rxCallback != NULL)
            {
                uartState->rxCallback(uartState, UART_EVENT_RX_FULL, uartState->rxCallbackParam);
            }
        }

        /* Finish reception if this was the last byte received */
        if (uartState->rxSize == 0U)
        {
            /* Complete transfer (disable rx logic) */
            UART_DRV_CompleteReceiveDataUsingInt(instance);

            /* Invoke callback if there is one */
            if (uartState->rxCallback != NULL)
            {
                uartState->rxCallback(uartState, UART_EVENT_END_TRANSFER, uartState->rxCallbackParam);
            }
        }
    }
}

/*!
 * @brief TX data-register-empty interrupt sub-handler.
 *
 * Fills the TX FIFO (or DATA register) from the state buffer and switches
 * to TX-complete interrupt when the buffer is exhausted.
 *
 * @param[in] instance  UART instance index (0-based).
 */
static void UART_DRV_TxEmptyIrqHandler(uint32_t instance)
{
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];
    uint32_t txFifoEmptyCount;
    bool status = false;

#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
    uint32_t txFifoWordCount;
    /* Get txfifo word count */
    txFifoWordCount = UART_GetWordCountInTxFifo(base);
    /* Get empty count in tx fifo */
    txFifoEmptyCount = UART_GetTxFifoSize(base) - txFifoWordCount;
#else
    txFifoEmptyCount = 1;
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

    for (uint32_t i = 0; i < txFifoEmptyCount; i++)
    {
        /* Check if there are any more bytes to send */
        if (uartState->txSize > 0U)
        {
            /* Transmit the data */
            UART_DRV_PutData(instance);

            /* Update the internal state */
            if (uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR)
            {
                ++uartState->txBuff;
                --uartState->txSize; /* PRQA S 3387 */
            }
            else
            {
                uartState->txBuff = &uartState->txBuff[2];
                uartState->txSize -= 2U;
            }

            /* Check if this was the last byte in the current buffer */
            if (uartState->txSize == 0U)
            {
                /* Invoke callback if there is one (callback may reset the tx buffer for continuous transmission)*/
                if (uartState->txCallback != NULL)
                {
                    uartState->txCallback(uartState, UART_EVENT_TX_EMPTY, uartState->txCallbackParam);
                }
                
                /* If there's no new data, disable tx empty interrupt and enable transmission complete interrupt */
                if (uartState->txSize == 0U)
                {
                    UART_SetIntMode(base, UART_INT_TX_DATA_REG_EMPTY, false);
                    UART_SetIntMode(base, UART_INT_TX_COMPLETE, true);
                    status = true;
                }
            }
        }
        else
        {
            status = true;
        }

        if (status)
        {
            break;
        }
    }
}

/*!
 * @brief TX-complete interrupt sub-handler.
 *
 * Called after the last byte has been fully shifted out.  Completes the
 * transfer and invokes the user callback with UART_EVENT_END_TRANSFER.
 *
 * @param[in] instance  UART instance index (0-based).
 */
static void UART_DRV_TxCompleteIrqHandler(uint32_t instance)
{
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    if (uartState->txSize == 0U)
    {
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            /* Complete the interrupt based transfer */
            UART_DRV_CompleteSendDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            /* Complete the DMA based transfer */
            UART_DRV_StopTxDma(instance);
        }
#endif
        /* Invoke callback if there is one */
        if (uartState->txCallback != NULL)
        {
            uartState->txCallback(uartState, UART_EVENT_END_TRANSFER, uartState->txCallbackParam);
        }
    }
}

/*!
 * @brief Error / idle-line interrupt sub-handler.
 *
 * Processes framing, noise, parity, and overrun errors as well as the
 * idle-line detection flag.  On error, the reception is aborted and the
 * user callback is invoked with the appropriate error event.
 *
 * @param[in] instance  UART instance index (0-based).
 */
static void UART_DRV_ErrIrqHandler(uint32_t instance)
{
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];

    /* Handle IDLE interrupt */
    if (UART_GetIntMode(base, UART_INT_IDLE_LINE))
    {
        if (UART_GetStatusFlag(base, UART_IDLE_LINE_DETECT))
        {
            (void)UART_ClearStatusFlag(base, UART_IDLE_LINE_DETECT);
            /* Update the internal status */
            uartState->receiveStatus = STATUS_UART_IDLE_ERROR;
            /* Invoke callback if there is one */
            if (uartState->rxCallback != NULL)
            {
                uartState->rxCallback(uartState, UART_EVENT_ERROR, uartState->rxCallbackParam);
            }
        }
    }
    /* Handle receive overrun interrupt */
    if (UART_GetStatusFlag(base, UART_RX_OVERRUN))
    {
        (void)UART_ClearStatusFlag(base, UART_RX_OVERRUN);
        /* Update the internal status */
        uartState->receiveStatus = STATUS_UART_RX_OVERRUN;
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            /* Complete the transfer (disable rx logic) */
            UART_DRV_CompleteReceiveDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            /* Complete the transfer (stop DMA channel) */
            UART_DRV_StopRxDma(instance);
        }
#endif
        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, UART_EVENT_ERROR, uartState->rxCallbackParam);
        }
    }
    /* Handle framing error interrupt */
    if (UART_GetStatusFlag(base, UART_FRAME_ERR))
    {
        (void)UART_ClearStatusFlag(base, UART_FRAME_ERR);
        /* Update the internal status */
        uartState->receiveStatus = STATUS_UART_FRAMING_ERROR;
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            /* Complete the transfer (disable rx logic) */
            UART_DRV_CompleteReceiveDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            /* Complete the transfer (stop DMA channel) */
            UART_DRV_StopRxDma(instance);
        }
#endif
        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, UART_EVENT_ERROR, uartState->rxCallbackParam);
        }
    }
    /* Handle parity error interrupt */
    if (UART_GetStatusFlag(base, UART_PARITY_ERR))
    {
        (void)UART_ClearStatusFlag(base, UART_PARITY_ERR);
        /* Update the internal status */
        uartState->receiveStatus = STATUS_UART_PARITY_ERROR;
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            /* Complete the transfer (disable rx logic) */
            UART_DRV_CompleteReceiveDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            /* Complete the transfer (stop DMA channel) */
            UART_DRV_StopRxDma(instance);
        }
#endif
        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, UART_EVENT_ERROR, uartState->rxCallbackParam);
        }
    }
    /* Handle noise error interrupt */
    if (UART_GetStatusFlag(base, UART_NOISE_DETECT))
    {
        (void)UART_ClearStatusFlag(base, UART_NOISE_DETECT);
        /* Update the internal status */
        uartState->receiveStatus = STATUS_UART_NOISE_ERROR;
        if (uartState->transferType == UART_USING_INTERRUPTS)
        {
            /* Complete transfer (disable rx logic) */
            UART_DRV_CompleteReceiveDataUsingInt(instance);
        }
#if FEATURE_UART_HAS_DMA_ENABLE
        else
        {
            /* Complete the transfer (stop DMA channel) */
            UART_DRV_StopRxDma(instance);
        }
#endif
        /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, UART_EVENT_ERROR, uartState->rxCallbackParam);
        }
    }
}

/*!
 * @brief Start interrupt-based transmission.
 *
 * Configures state, enables the transmitter and TX-empty interrupt.
 *
 * @param[in] instance  UART instance index.
 * @param[in] txBuff    Transmit buffer pointer.
 * @param[in] txSize    Number of bytes to send.
 * @return STATUS_SUCCESS or STATUS_BUSY.
 */
static status_t UART_DRV_StartSendDataUsingInt(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retVal = STATUS_SUCCESS;

    /* Check it's not busy transmitting data from a previous function call */
    if (uartState->isTxBusy)
    {
        retVal = STATUS_BUSY;
    }
    if (retVal != STATUS_BUSY)
    {
            /* Check the validity of the parameters */
        DEV_ASSERT(txSize > 0U);
        DEV_ASSERT((uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR) || ((txSize & 1U) == 0U));

        /* initialize the module driver state structsure */
        uartState->txBuff = txBuff;
        uartState->txSize = txSize;
        uartState->isTxBusy = true;
        uartState->transmitStatus = STATUS_BUSY;
#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
        /* initialize tx */
        UART_ResetTxFifo(base);
        UART_SetTxFifoWatermark(base, UART_DEFAULT_TX_FIFO_WATERMARK);
        UART_EnableTxFifo(base, true);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

        /* Enable the UART transmitter */
        UART_SetTransmitterCmd(base, true);

        /* Enable tx empty interrupt */
        UART_SetIntMode(base, UART_INT_TX_DATA_REG_EMPTY, true);
    }

    return retVal;
}

#if FEATURE_UART_HAS_DMA_ENABLE
/*!
 * @brief Start DMA-based transmission.
 *
 * Configures the DMA transfer descriptor, installs the completion callback,
 * and enables the transmitter with DMA requests.
 *
 * @param[in] instance  UART instance index.
 * @param[in] txBuff    Transmit buffer pointer.
 * @param[in] txSize    Number of bytes to send.
 * @return STATUS_SUCCESS or STATUS_BUSY.
 */
static status_t UART_DRV_StartSendDataUsingDma(uint32_t instance, const uint8_t *txBuff, uint32_t txSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(txBuff != NULL);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retval = STATUS_SUCCESS;

    /* Check it's not busy transmitting data from a previous function call */
    if (uartState->isTxBusy)
    {
        retval = STATUS_BUSY;
    }
    
    if (retval != STATUS_BUSY)
    {
        DEV_ASSERT(txSize > 0U);

        /* Update state structure */
        uartState->txBuff = txBuff;
        uartState->txSize = 0U;
        uartState->isTxBusy = true;
        uartState->transmitStatus = STATUS_BUSY;

#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
        /* initialize tx */
        UART_ResetTxFifo(base);
        UART_SetTxFifoWatermark(base, UART_DEFAULT_TX_FIFO_WATERMARK);
        UART_EnableTxFifo(base, true);
#endif

        /* Configure the transfer control descriptor for the previously allocated channel */
        (void)DMA_DRV_ConfigMultiBlockTransfer(uartState->txDMAChannel,
                                               DMA_TRANSFER_MEM2PERIPH,
                                               (uint32_t)txBuff,
                                               (uint32_t)(&(base->DATA)),
                                               DMA_TRANSFER_SIZE_1B,
                                               1U,
                                               txSize,
                                               true);

        /* Call driver function to end the transmission when the DMA transfer is done */
        (void)DMA_DRV_InstallCallback(uartState->txDMAChannel,
                                      (dma_callback_t)(UART_DRV_TxDmaCallback),
                                      (void *)(instance)); /* PRQA S 0326 */

        /* Start the DMA channel */
        (void)DMA_DRV_StartChannel(uartState->txDMAChannel);

        /* Enable the UART transmitter */
        UART_SetTransmitterCmd(base, true);

        /* Enable tx DMA requests for the current instance */
        UART_SetTxDmaCmd(base, true);
    }

    return retval;
}
#endif

/*!
 * @brief Complete an interrupt-based transmission.
 *
 * Disables TX interrupts, disables the transmitter, and signals the
 * blocking semaphore if applicable.
 *
 * @param[in] instance  UART instance index.
 */
static void UART_DRV_CompleteSendDataUsingInt(uint32_t instance)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    if (uartState->transmitStatus == STATUS_BUSY)
    {
        /* If the transfer is completed, update the transmit status */
        uartState->transmitStatus = STATUS_SUCCESS;
    }
    else
    {
        /* If the transfer is aborted or timed out, disable tx empty interrupt */
        UART_SetIntMode(base, UART_INT_TX_DATA_REG_EMPTY, false);
    }

    /* Disable transmission complete interrupt */
    UART_SetIntMode(base, UART_INT_TX_COMPLETE, false);

    /* Disable transmitter */
    UART_SetTransmitterCmd(base, false);

    /* Update the internal busy flag */
    uartState->isTxBusy = false;

    /* Signal the synchronous completion object. */
    if (uartState->isTxBlocking)
    {
        (void)OSIF_SemaPost(&uartState->txComplete);
    }
}

#if FEATURE_UART_HAS_DMA_ENABLE
/*!
 * @brief DMA TX completion callback.
 *
 * Called by the DMA driver when the TX trigger-loop completes. Handles
 * continuous-transmit chaining or enables TX-complete interrupt.
 *
 * @param[in] parameter  UART instance index (cast to void *).
 * @param[in] status     DMA channel completion status.
 */
static void UART_DRV_TxDmaCallback(void *parameter, dma_chn_status_t status)
{
    uint32_t instance = ((uint32_t)parameter); /* PRQA S 0326 */
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];

    /* Check if the DMA transfer completed with errors */
    if (status == DMA_CHN_ERROR)
    {
        /* Update the status */
        uartState->transmitStatus = STATUS_ERROR;
        /* Stop the transfer */
        UART_DRV_StopTxDma(instance);
        /* Notify the application that an error occurred */
        if (uartState->txCallback != NULL)
        {
            uartState->txCallback(uartState, UART_EVENT_ERROR, uartState->txCallbackParam);
        }
    }
    else
    {
        /* Invoke callback if there is one */
        if (uartState->txCallback != NULL)
        {
            /* Allow the user to provide a new buffer, for continuous transmission */
            uartState->txCallback(uartState, UART_EVENT_TX_EMPTY, uartState->txCallbackParam);
        }

        /* If the callback has updated the tx buffer, update the DMA descriptor to continue the transfer;
         * otherwise, stop the current transfer.
         */
        if (uartState->txSize > 0U)
        {
            /* Set the source address and the number of transfer loops (bytes to be transfered) */
            DMA_DRV_SetSrcAddr(uartState->txDMAChannel, (uint32_t)(uartState->txBuff));
            DMA_DRV_SetTriggerLoopIterationCount(uartState->txDMAChannel, uartState->txSize);

            /* Now that this tx is set up, clear remaining bytes count */
            uartState->txSize = 0U;

            /* Re-start the channel */
            (void)DMA_DRV_StartChannel(uartState->txDMAChannel);
        }
        else
        {
            /* Enable transmission complete interrupt */
            UART_SetIntMode(base, UART_INT_TX_COMPLETE, true);
        }
    }
}
#endif

/*!
 * @brief Start interrupt-based reception.
 *
 * Configures state, enables the receiver, error interrupts, and
 * RX-data-full interrupt.
 *
 * @param[in]  instance  UART instance index.
 * @param[out] rxBuff    Receive buffer pointer.
 * @param[in]  rxSize    Number of bytes to receive.
 * @return STATUS_SUCCESS or STATUS_BUSY.
 */
static status_t UART_DRV_StartReceiveDataUsingInt(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);
    DEV_ASSERT(rxSize != 0);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];
    status_t retval = STATUS_SUCCESS;

    /* Check it's not busy receiving data from a previous function call */
    if (uartState->isRxBusy)
    {
        retval = STATUS_BUSY;
    }

    if (retval != STATUS_BUSY)
    {
        /* Check the validity of the parameters */
        DEV_ASSERT(rxSize > 0U);
        DEV_ASSERT((uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR) || ((rxSize & 1U) == 0U));

        /* Initialize the module driver state struct to indicate transfer in progress
        * and with the buffer and byte count data. */
        uartState->isRxBusy = true;
        uartState->rxBuff = rxBuff;
        uartState->rxSize = rxSize;
        uartState->receiveStatus = STATUS_BUSY;
#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
        /* Initialize rx fifo */
        UART_ResetRxFifo(base);
        UART_SetRxFifoWatermark(base, 0);
        UART_EnableRxFifo(base, true);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

        /* Enable the receiver */
        UART_SetReceiverCmd(base, true);

        /* Enable error interrupts */
        UART_DRV_SetErrorInterrupts(instance, true);

        /* Enable receive data full interrupt */
        UART_SetIntMode(base, UART_INT_RX_DATA_REG_FULL, true);
    }

    return retval;
}

#if FEATURE_UART_HAS_DMA_ENABLE
/*!
 * @brief Start DMA-based reception.
 *
 * Configures the DMA transfer descriptor, installs the completion callback,
 * and enables the receiver with DMA requests.
 *
 * @param[in]  instance  UART instance index.
 * @param[out] rxBuff    Receive buffer pointer.
 * @param[in]  rxSize    Number of bytes to receive.
 * @return STATUS_SUCCESS or STATUS_BUSY.
 */
static status_t UART_DRV_StartReceiveDataUsingDma(uint32_t instance, uint8_t *rxBuff, uint32_t rxSize)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);
    DEV_ASSERT(rxBuff != NULL);

    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    status_t retval = STATUS_SUCCESS;

    /* Check it's not busy receiving data from a previous function call */
    if (uartState->isRxBusy)
    {
        retval = STATUS_BUSY;
    }

    if (retval != STATUS_BUSY)
    {
        DEV_ASSERT(rxSize > 0U);

        /* Configure the transfer control descriptor for the previously allocated channel */
        (void)DMA_DRV_ConfigMultiBlockTransfer(uartState->rxDMAChannel,
                                               DMA_TRANSFER_PERIPH2MEM,
                                               (uint32_t)(&(base->DATA)),
                                               (uint32_t)rxBuff,
                                               DMA_TRANSFER_SIZE_1B,
                                               1U,
                                               rxSize,
                                               true);

        /* Call driver function to end the reception when the DMA transfer is done */
        (void)DMA_DRV_InstallCallback(uartState->rxDMAChannel,
                                      (dma_callback_t)(UART_DRV_RxDmaCallback),
                                      (void *)(instance)); /* PRQA S 0326 */

        /* Start the DMA channel */
        (void)DMA_DRV_StartChannel(uartState->rxDMAChannel);

        /* Update the state structure */
        uartState->rxBuff = rxBuff;
        uartState->rxSize = 0U;
        uartState->isRxBusy = true;
        uartState->receiveStatus = STATUS_BUSY;

#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
        /* Initialize rx fifo */
        UART_ResetRxFifo(base);
        UART_SetRxFifoWatermark(base, 0);
        UART_EnableRxFifo(base, true);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

        /* Enable the receiver */
        UART_SetReceiverCmd(base, true);

        /* Enable error interrupts */
        UART_DRV_SetErrorInterrupts(instance, true);

        /* Enable rx DMA requests for the current instance */
        UART_SetRxDmaCmd(base, true);
    }

    return retval;
}
#endif

/*!
 * @brief Complete an interrupt-based reception.
 *
 * Disables the receiver, error and RX-full interrupts, and signals the
 * blocking semaphore if applicable.
 *
 * @param[in] instance  UART instance index.
 */
static void UART_DRV_CompleteReceiveDataUsingInt(uint32_t instance)
{
    DEV_ASSERT(instance < UART_INSTANCE_COUNT);

    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];

    /* Disable receiver */
    UART_SetReceiverCmd(base, false);

    /* Disable error interrupts */
    UART_DRV_SetErrorInterrupts(instance, false);
#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
    /* Reset rx fifo to clear RDRF flag */
    UART_ResetRxFifo(base);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

    /* Disable receive data full and rx overrun interrupt. */
    UART_SetIntMode(base, UART_INT_RX_DATA_REG_FULL, false);

    /* Signal the synchronous completion object. */
    if (uartState->isRxBlocking)
    {
        (void)OSIF_SemaPost(&uartState->rxComplete);
        uartState->isRxBlocking = false;
    }

    /* Update the information of the module driver state */
    uartState->isRxBusy = false;
    if (uartState->receiveStatus == STATUS_BUSY)
    {
        uartState->receiveStatus = STATUS_SUCCESS;
    }
}

#if FEATURE_UART_HAS_DMA_ENABLE
/*!
 * @brief DMA RX completion callback.
 *
 * Called by the DMA driver when the RX trigger-loop completes.
 *
 * @param[in] parameter  UART instance index (cast to void *).
 * @param[in] status     DMA channel completion status.
 */
static void UART_DRV_RxDmaCallback(void *parameter, dma_chn_status_t status)
{
    uint32_t instance = ((uint32_t)parameter); /* PRQA S 0326 */
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];

    if (status == DMA_CHN_ERROR)
    {
        /* Update the status */
        uartState->receiveStatus = STATUS_ERROR;
        /* Stop the transfer */
        UART_DRV_StopRxDma(instance);
        /* Notify the application that an error occurred */
        if (uartState->rxCallback != NULL)
        {
            uartState->rxCallback(uartState, UART_EVENT_ERROR, uartState->rxCallbackParam);
        }

        /* Clear the flags */
        UART_DRV_ClearErrorFlags(base);
    }

    /* Return if an error occurred; error cases are treated by the interrupt handler */
    if (uartState->receiveStatus != STATUS_BUSY)
    {
        /* Nothing to do */
    }
    else
    {
            /* Invoke callback if there is one */
        if (uartState->rxCallback != NULL)
        {
            /* Allow the user to provide a new buffer inside the callback, to continue the reception */
            uartState->rxCallback(uartState, UART_EVENT_RX_FULL, uartState->rxCallbackParam);
        }

        /* If the callback has updated the rx buffer, update the DMA descriptor to continue the transfer;
        * otherwise, stop the current transfer.
        */
        if (uartState->rxSize > 0U)
        {
            /* Set the destination address and the number of transfer loops (bytes to be transfered) */
            DMA_DRV_SetDestAddr(uartState->rxDMAChannel, (uint32_t)(uartState->rxBuff));
            DMA_DRV_SetTriggerLoopIterationCount(uartState->rxDMAChannel, uartState->rxSize);

            /* Now that this rx is set up, clear remaining bytes count */
            uartState->rxSize = 0U;

            /* Re-start the channel */
            (void)DMA_DRV_StartChannel(uartState->rxDMAChannel);
        }
        else
        {
            /* Stop the reception */
            UART_DRV_StopRxDma(instance);

            /* Invoke the callback to notify the end of the transfer */
            if (uartState->rxCallback != NULL)
            {
                uartState->rxCallback(uartState, UART_EVENT_END_TRANSFER, uartState->rxCallbackParam);
            }

            /* Clear the flags */
            UART_DRV_ClearErrorFlags(base);
        }
    }
}
#endif

/*!
 * @brief Write one data word to the transmit register.
 *
 * Handles 7/8-bit, 9-bit, and 10-bit character widths.
 *
 * @param[in] instance  UART instance index.
 */
static void UART_DRV_PutData(uint32_t instance)
{
    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    UART_Type *base = s_uartBase[instance];
    uint16_t data;
    const uint8_t *txBuff = uartState->txBuff;

    if (uartState->bitCountPerChar <= UART_8_BITS_PER_CHAR)
    {
        /* Transmit the data */
        UART_Putchar(base, *txBuff);
    }
    else if (uartState->bitCountPerChar == UART_9_BITS_PER_CHAR)
    {
        /* Create a 16-bits integer from two bytes */
        data = (uint16_t)(*txBuff);
        ++txBuff;
        data |= (uint16_t)(((uint16_t)(*txBuff)) << 8U);

        /* Transmit the data */
        UART_Putchar9(base, data);
    }
    else
    {
        /* Create a 16-bits integer from two bytes */
        data = (uint16_t)(*txBuff);
        ++txBuff;
        data |= (uint16_t)(((uint16_t)(*txBuff)) << 8U);

        /* Transmit the data */
        UART_Putchar10(base, data);
    }
}

/*!
 * @brief Read one data word from the receive register.
 *
 * Handles 7-bit, 8-bit, 9-bit, and 10-bit character widths.
 *
 * @param[in] instance  UART instance index.
 */
static void UART_DRV_GetData(uint32_t instance)
{
    const uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    const UART_Type *base = s_uartBase[instance];
    uint16_t data;
    uint8_t *rxBuff = uartState->rxBuff;

    if (uartState->bitCountPerChar == UART_7_BITS_PER_CHAR)
    {
        /* Receive the data */
        UART_Getchar7(base, rxBuff);
    }
    else if (uartState->bitCountPerChar == UART_8_BITS_PER_CHAR)
    {
        UART_Getchar8(base, rxBuff);
    }
    else if (uartState->bitCountPerChar == UART_9_BITS_PER_CHAR)
    {
        /* Receive the data */
        UART_Getchar9(base, &data);

        /* Write the least significant bits to the receive buffer */
        *rxBuff = (uint8_t)(data & 0xFFU);
        ++rxBuff;
        /* Write the ninth bit to the subsequent byte in the rx buffer */
        *rxBuff = (uint8_t)(data >> 8U);
    }
    else
    {
        /* Receive the data */
        UART_Getchar10(base, &data);

        /* Write the least significant bits to the receive buffer */
        *rxBuff = (uint8_t)(data & 0xFFU);
        ++rxBuff;
        /* Write the ninth and tenth bits to the subsequent byte in the rx buffer */
        *rxBuff = (uint8_t)(data >> 8U);
    }
}

#if FEATURE_UART_HAS_DMA_ENABLE
/*!
 * @brief Stop a DMA-based transmission.
 *
 * Disables DMA requests, TX-complete interrupt, disables the transmitter,
 * and signals the blocking semaphore if applicable.
 *
 * @param[in] instance  UART instance index.
 */
static void UART_DRV_StopTxDma(uint32_t instance)
{
    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Disable tx DMA requests for the current instance */
    UART_SetTxDmaCmd(base, false);

    /* Stop the dma channel */
    (void)DMA_DRV_StopChannel(uartState->txDMAChannel);

    /* Disable transmission complete interrupt */
    UART_SetIntMode(base, UART_INT_TX_COMPLETE, false);

    /* Disable transmitter */
    UART_SetTransmitterCmd(base, false);

    /* Signal the synchronous completion object. */
    if (uartState->isTxBlocking)
    {
        (void)OSIF_SemaPost(&uartState->txComplete);
    }

    if (uartState->transmitStatus == STATUS_BUSY)
    {
        /* If the transfer is completed, update the transmit status */
        uartState->transmitStatus = STATUS_SUCCESS;
    }

    /* Update the internal busy flag */
    uartState->isTxBusy = false;
}

/*!
 * @brief Stop a DMA-based reception.
 *
 * Disables the receiver, error interrupts, DMA requests, and signals the
 * blocking semaphore if applicable.
 *
 * @param[in] instance  UART instance index.
 */
static void UART_DRV_StopRxDma(uint32_t instance)
{
    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];

    /* Disable receiver */
    UART_SetReceiverCmd(base, false);

    /* Disable error interrupts */
    UART_DRV_SetErrorInterrupts(instance, false);

    /* Disable rx DMA requests for the current instance */
    UART_SetRxDmaCmd(base, false);

#if defined(FEATURE_UART_FIFO_SIZE) && FEATURE_UART_FIFO_SIZE > 0
    /* Reset rx fifo to clear RDRF flag */
    UART_ResetRxFifo(base);
#endif /* FEATURE_UART_FIFO_SIZE > 0 */

    /* Stop the DMA channel */
    (void)DMA_DRV_StopChannel(uartState->rxDMAChannel);

    /* Signal the synchronous completion object. */
    if (uartState->isRxBlocking)
    {
        (void)OSIF_SemaPost(&uartState->rxComplete);
        uartState->isRxBlocking = false;
    }

    /* Update the internal driver status */
    if (uartState->receiveStatus == STATUS_BUSY)
    {
        uartState->receiveStatus = STATUS_SUCCESS;
    }

    /* Update the information of the module driver state */
    uartState->isRxBusy = false;
}
#endif

/*!
 * @brief Enable or disable UART error interrupts (framing, noise, parity, overrun, idle).
 *
 * @param[in] instance  UART instance index.
 * @param[in] enable    true = enable, false = disable.
 */
static void UART_DRV_SetErrorInterrupts(uint32_t instance, bool enable)
{
    UART_Type *base = s_uartBase[instance];
    uart_state_t *uartState = (uart_state_t *)s_uartStatePtr[instance];
    bool idleIntEnable = uartState->idleErrorIntEnable;

    /* Disable/Enable error interrupts */
    UART_SetErrorInterrupts(base, enable);
    UART_SetIntMode(base, UART_INT_IDLE_LINE, idleIntEnable && enable);
}

#if FEATURE_UART_HAS_MODEM_SUPPORT
/*!
 * @brief Configure UART CTS (Clear To Send) for transmit flow control.
 *
 * @param[in] instance         UART instance index.
 * @param[in] uartTxCtsConfig  Pointer to CTS configuration structure.
 */
void UART_DRV_SetTransmitCTS(uint32_t instance, uart_tx_cts_config_t *uartTxCtsConfig)
{
    UART_Type *base = s_uartBase[instance];

    UART_SetTxCtsSource(base, uartTxCtsConfig->txCtsSrc);
    UART_SetTxCtsDetectionTime(base, uartTxCtsConfig->txCtsDetTime);
    UART_EnableTxCts(base, uartTxCtsConfig->txCtsEnable);
}

/*!
 * @brief Configure UART receive RTS (Request To Send) flow control.
 *
 * @param[in] instance         UART instance index.
 * @param[in] uartRxRtsConfig  Pointer to RX-RTS configuration structure.
 */
void UART_DRV_SetReceiveRTS(uint32_t instance, uart_rx_rts_config_t *uartRxRtsConfig)
{
    UART_Type *base = s_uartBase[instance];

    DEV_ASSERT(uartRxRtsConfig->rxRtsWatermark < FEATURE_UART_FIFO_SIZE);
    DEV_ASSERT(!UART_IsEnableTxRts(base));

    UART_SetRxRtsWatermark(base, uartRxRtsConfig->rxRtsWatermark);
    UART_EnableRxRts(base, uartRxRtsConfig->rxRtsEnbale);
}

/*!
 * @brief Configure UART transmit RTS polarity and enable.
 *
 * @param[in] instance         UART instance index.
 * @param[in] uartTxRtsConfig  Pointer to TX-RTS configuration structure.
 */
void UART_DRV_SetTransmitRTS(uint32_t instance, uart_tx_rts_config_t *uartTxRtsConfig)
{
    UART_Type *base = s_uartBase[instance];

    DEV_ASSERT(!UART_IsEnableRxRts(base));

    UART_SetTxRtsPolarity(base, uartTxRtsConfig->txRtsPolarity);
    UART_EnableTxRts(base, uartTxRtsConfig->txRtsEnable);
}
#endif

#if defined(FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT) && (FEATURE_UART_HAS_ONE_WIRE_MODE_SUPPORT == 1U) 
/*!
 * @brief Configure UART single-wire (one-wire) mode.
 *
 * When enabled, activates loop mode and sets the TX line direction.
 *
 * @param[in] instance           UART instance index.
 * @param[in] oneWireModeConfig  Pointer to one-wire mode configuration.
 */
void UART_DRV_SetOneWireMode(uint32_t instance, uart_one_wire_mode_config_t *oneWireModeConfig)
{
    UART_Type *base = s_uartBase[instance];

    if (oneWireModeConfig->oneWireModeEnable)
    {
        UART_EnableLoopMode(base, true);
        UART_EnableOneWireMode(base, true);
        UART_DRV_SetTxLineDirection(instance, oneWireModeConfig->txLineDirection);
    }
    else
    {
        UART_EnableLoopMode(base, false);
        UART_EnableOneWireMode(base, false);
    }
}

/*!
 * @brief Set the TX line direction in one-wire mode.
 *
 * @param[in] instance        UART instance index.
 * @param[in] txLinDerection  TX line direction (input / output).
 */
void UART_DRV_SetTxLineDirection(uint32_t instance, uart_tx_line_derection_t txLinDerection)
{
    UART_Type *base = s_uartBase[instance];

    UART_SetTxLineDirection(base, txLinDerection);
}
#endif

/*******************************************************************************
 * EOF
 ******************************************************************************/

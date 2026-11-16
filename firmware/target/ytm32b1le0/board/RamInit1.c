/*
 * Copyright 2020-2026 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file RamInit1.c
 * @brief 
 * 
 */

#include <stdint.h>
#include <stdbool.h>
#include "device_registers.h"

#define WORD_ALIGN_MASK (3U)
#define WORD_SHIFT      (2U)

typedef enum
{
    INIT_NULL = 0U,   /*!< No initialization */
    INIT_NORMAL = 1U, /*!< Normal initialization */
    INIT_POR = 2U,    /*!< Power on reset initialization */
} RamInitType;


typedef struct
{
    uint32_t *RamStart;   /*!< Start address of section in RAM */
    uint32_t *RomStart;   /*!< Start address of section in ROM */
    uint32_t *RomEnd;     /*!< End address of section in ROM */
    RamInitType InitType;   /*!< Type of initialization*/
} RamCopyLayoutType;

typedef struct
{
    uint32_t *RamStart;   /*!< Start address of section in RAM */
    uint32_t *RamEnd;     /*!< End address of section in RAM */
    RamInitType InitType;   /*!< Type of initialization*/
} RamZeroLayoutType;

extern uint32_t Image$$CODE_RAM_start$$Base;
extern uint32_t Load$$CODE_RAM_start$$Base;
extern uint32_t Load$$CODE_RAM_end$$Limit;
extern uint32_t Image$$DATA_RAM_start$$Base;
extern uint32_t Load$$DATA_RAM_start$$Base;
extern uint32_t Load$$DATA_RAM_end$$Limit;

extern uint32_t Image$$BSS_start$$Base;
extern uint32_t Image$$BSS_end$$Limit;

const RamCopyLayoutType CopyLayout[2]={
    {
        .RamStart=(uint32_t *)&Image$$CODE_RAM_start$$Base,
        .RomStart=(uint32_t *)&Load$$CODE_RAM_start$$Base,
        .RomEnd=(uint32_t *)&Load$$CODE_RAM_end$$Limit,
        .InitType=INIT_NORMAL,
    },
    {
        .RamStart=(uint32_t *)&Image$$DATA_RAM_start$$Base,
        .RomStart=(uint32_t *)&Load$$DATA_RAM_start$$Base,
        .RomEnd=(uint32_t *)&Load$$DATA_RAM_end$$Limit,
        .InitType=INIT_NORMAL,
    },
};

const RamZeroLayoutType ZeroLayout[1]={
    {
        .RamStart=(uint32_t *)&Image$$BSS_start$$Base,
        .RamEnd=(uint32_t *)&Image$$BSS_end$$Limit,
        .InitType=INIT_NORMAL,
    },
};


/**
 * @brief Check MCU is power on reset or not, user can override this function
 * @return 1: POR, 0: Not POR
 */
#define RCU_RSSR_POR_MASK   (0x1U)
static bool IsPOR(void)
{
    bool RetVal = false;
    /* Check if the MCU is power on reset or not */
    if (RCU_RSSR_POR_MASK == (RCU->RSSR & RCU_RSSR_POR_MASK))
    {
        RetVal = true;
    }

    return RetVal;
}


/**
 * @brief RamInit1 for copying initialized data and zeroing uninitialized data
 */
void RamInit1(){
    bool IsPor = IsPOR();
    const uint32_t *Rom32;
    const uint8_t *Rom8;
    uint32_t *Ram32;
    uint8_t *Ram8;
    volatile uint8_t DataPad;
    volatile uint32_t Len = 0U;
    uint32_t Size = 0U;
    uint32_t MajorLoopCnt = 0U;
    uint32_t MinorLoopCnt = 0U;

    /* Copy initialized table */
    Len = 2;
    for (MajorLoopCnt = 0U; MajorLoopCnt < Len; ++MajorLoopCnt)
    {
        if(((CopyLayout[MajorLoopCnt].InitType == INIT_POR) && (IsPor != true)) || (CopyLayout[MajorLoopCnt].InitType == INIT_NULL))
        {
            continue;
        }
        Rom32 = (const uint32_t *)CopyLayout[MajorLoopCnt].RomStart;
        Ram32 = (uint32_t *)CopyLayout[MajorLoopCnt].RamStart;
        Size = (uint32_t)CopyLayout[MajorLoopCnt].RomEnd - (uint32_t)CopyLayout[MajorLoopCnt].RomStart;
        /* Make sure the data area to be copied must be aligned with 4. Then, copy 4 bytes at per one read */
        DataPad = Size & WORD_ALIGN_MASK;
        Size = (Size - DataPad) >> WORD_SHIFT;
        for (MinorLoopCnt = 0U; MinorLoopCnt < Size; ++MinorLoopCnt)
        {
            Ram32[MinorLoopCnt] = Rom32[MinorLoopCnt];
        }
        /* For the rest of data, copy 1 bytes at per one read */
        Rom8 = (uint8_t *)(&Rom32[MinorLoopCnt]);
        Ram8 = (uint8_t *)(&Ram32[MinorLoopCnt]);
        for (MinorLoopCnt = 0U; MinorLoopCnt < DataPad; ++MinorLoopCnt)
        {
            Ram8[MinorLoopCnt] = Rom8[MinorLoopCnt];
        }
    }

    /* Clear zero table */
    Len = 1;
    for (MajorLoopCnt = 0U; MajorLoopCnt < Len; ++MajorLoopCnt)
    {
        if(((ZeroLayout[MajorLoopCnt].InitType == INIT_POR) && (IsPor != true)) || (ZeroLayout[MajorLoopCnt].InitType == INIT_NULL))
        {
            continue;
        }
        Ram32 = (uint32_t *)ZeroLayout[MajorLoopCnt].RamStart;
        Size = (uint32_t)ZeroLayout[MajorLoopCnt].RamEnd - (uint32_t)ZeroLayout[MajorLoopCnt].RamStart;
        /* Make sure the data area to be zeroed must be aligned with 4. Then, zero 4 bytes at per one read */
        DataPad = Size & WORD_ALIGN_MASK;
        Size = (Size - DataPad) >> WORD_SHIFT;        
        for (MinorLoopCnt = 0U; MinorLoopCnt < Size; ++MinorLoopCnt)
        {
            Ram32[MinorLoopCnt] = 0U;
        }
        /* For the rest of data, zero 1 bytes at per one read */
        Ram8 = (uint8_t *)(&Ram32[MinorLoopCnt]);
        for (MinorLoopCnt = 0U; MinorLoopCnt < DataPad; ++MinorLoopCnt)
        {
            Ram8[MinorLoopCnt] = 0U;
        }
    }
}



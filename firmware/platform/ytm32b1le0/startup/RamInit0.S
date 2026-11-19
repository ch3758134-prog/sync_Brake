/*
 * Copyright 2020-2026 Yuntu Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * @file RamInit0.S
 * @brief 
 * 
 */


   PRESERVE8
   THUMB
   AREA    |.text|, CODE, READONLY
RamInit0
   EXPORT  RamInit0
_GET_POR
    LDR     R1, =0x4007F000
    LDR     R4, [R1]
    LDR     R3, =0x1
/* R4 store POR result */
    ANDS    R4, R3
_IVT_RAM_start
/* 8bytes align */
    LDR     R1, =0x20000000
    LDR     R2, =(0x20000000+0xC0)
    
    SUBS    R2, R2, R1
    SUBS    R2, #1
    BLE     _IVT_RAM_end

    LDR     R0, =0x5A5A5A5A
    LDR     R3, =0x5A5A5A5A

_IVT_RAM_loop
    STM     R1!, {R0,R3}
    SUBS    R2, #8
    BGE      _IVT_RAM_loop

_IVT_RAM_end

_STACK_start
/* 8bytes align */
    LDR     R1, =0x20003c00
    LDR     R2, =(0x20003c00+1024)
    
    SUBS    R2, R2, R1
    SUBS    R2, #1
    BLE     _STACK_end

    LDR     R0, =0x5A5A5A5A
    LDR     R3, =0x5A5A5A5A

_STACK_loop
    STM     R1!, {R0,R3}
    SUBS    R2, #8
    BGE      _STACK_loop

_STACK_end

_RAM_start
/* 8bytes align */
    LDR     R1, =0x200000C0
    LDR     R2, =(0x200000C0+0x3B40)
    
    SUBS    R2, R2, R1
    SUBS    R2, #1
    BLE     _RAM_end

    LDR     R0, =0x5A5A5A5A
    LDR     R3, =0x5A5A5A5A

_RAM_loop
    STM     R1!, {R0,R3}
    SUBS    R2, #8
    BGE      _RAM_loop

_RAM_end

   BX LR
   ALIGN 4
   END


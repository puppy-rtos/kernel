;/*
;* Copyright (c) 2006-2018, RT-Thread Development Team
;*
;* SPDX-License-Identifier: Apache-2.0
;*
; * Change Logs:
; * Date           Author       Notes
; * 2009-01-17     Bernard      first version.
; * 2012-01-01     aozima       support context switch load/store FPU register.
; * 2013-06-18     aozima       add restore MSP feature.
; * 2013-06-23     aozima       support lazy stack optimized.
; * 2018-07-24     aozima       enhancement hard fault exception handler.
; */

;/**
; * @addtogroup cortex-m4
; */
;/*@{*/

SCB_VTOR        EQU     0xE000ED08               ; Vector Table Offset Register
NVIC_INT_CTRL   EQU     0xE000ED04               ; interrupt control state register
NVIC_SYSPRI2    EQU     0xE000ED20               ; system priority register (2)
NVIC_PENDSV_PRI EQU     0xFFFF0000               ; PendSV and SysTick priority value (lowest)
NVIC_PENDSVSET  EQU     0x10000000               ; value to trigger PendSV exception

    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

    IMPORT arch_thread_to_ptr
    IMPORT _g_next_thread


; r0 --> switch from thread stack
; r1 --> switch to thread stack
; psr, pc, lr, r12, r3, r2, r1, r0 are pushed into [from] stack
PendSV_Handler   PROC
    EXPORT PendSV_Handler

    ; disable interrupt to protect context switch
    MRS     r2, PRIMASK
    CPSID   I

switch_to_thread
    LDR     r0, =_g_next_thread
    LDR     r0, [r0]                ; load thread stack pointer
    PUSH    {lr}
    BL      arch_thread_to_ptr
    POP     {lr}
    MOV     r1, r0

    MSR     psp, r1                 ; update stack pointer

pendsv_exit
    ; restore interrupt
    MSR     PRIMASK, r2

    ORR     lr, lr, #0x04
    BX      lr
    ENDP

    ALIGN   4

    END
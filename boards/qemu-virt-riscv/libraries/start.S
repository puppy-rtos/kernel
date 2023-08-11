/*
 * FreeRTOS V202212.01
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://www.github.com/FreeRTOS
 *
 */

    .section .init
    .globl _start
    .type _start,@function
_start:
    .cfi_startproc
    .cfi_undefined ra
.option push
.option norelax
    la  gp, __global_pointer$
.option pop

    // Continue primary hart
    csrr a0, mhartid
    li   a1, 0
    bne  a0, a1, secondary

    // Primary hart
    la sp, _stack_top
    csrw mscratch,sp

    // lw data section
    la a0, _data_lma
    la a1, _data
    la a2, _edata
    bgeu a1, a2, 2f
1:
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    bltu a1, a2, 1b
2:

    // Clear bss section
    la a0, _bss
    la a1, _ebss
    bgeu a0, a1, 2f
1:
    sw zero, (a0)
    addi a0, a0, 4
    bltu a0, a1, 1b
2:

    // argc, argv, envp is 0
    li  a0, 0
    li  a1, 0
    li  a2, 0
    jal main
1:
    wfi
    j 1b

secondary:
    csrr a0, mhartid
    li   a1, 1
    bne  a0, a1, cpu2
    // cpu1 hart
    la sp, cpu1_stack
    csrw mscratch,sp
    jal subcpu_entry
1:
    wfi
    j 1b
cpu2:
    csrr a0, mhartid
    li   a1, 2
    bne  a0, a1, cpuN
    // cpu1 hart
    la sp, cpu2_stack
    csrw mscratch,sp
    jal subcpu_entry
1:
    wfi
    j 1b
cpuN:
    wfi
    j cpuN
    .cfi_endproc

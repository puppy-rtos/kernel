/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <puppy/target.h>

P_WEAK int syscall_enter(int syscall_no)
{
    return syscall_no;
}
P_WEAK int syscall_leave(void)
{
    return 0;
}

__attribute__((naked)) void arch_syscall(int sycall_no, ...)
{
    __asm ("push {r4-r7,lr}");
    
    __asm ("push {r0-r3}");
    /* syscall enter, return syscall_no */
    __asm ("bl syscall_enter"); /* todo */
    __asm ("pop {r0-r3}");

    /* r1-r6: syscall args(r0-r5) */
    __asm ("add r7, sp, #20"); /* Remove push regs r4~r7,lr 4*5=20 */
    __asm ("ldmia r7,{r4-r6}");
    /* syscall args(r0-r5) push the reserved parameter r4,r5 position */
    __asm ("push {r5-r6}");
    
    /**
     * r0: syscall number
     * r1-r6: syscall args(r0-r5)
     */
    __asm ("svc 0");
    
    __asm ("push {r0-r3}");
    /* syscall enter, return syscall_no */
    __asm ("bl syscall_leave"); /* todo */
    __asm ("pop {r0-r3}");
    __asm ("pop {r4, r5}"); /* syscall args(r0-r5) Remove the reserved parameter position */
    __asm ("pop {r4-r7,pc}");
}

/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <puppy/target.h>

/**
 * r0~r3,r12,lr,psr caller regs ; s0~s15
 * r4~r11 called regs ; s16~s31
 * 
 */
bool arch_thread_in_kernel(p_obj_t obj)
{
    struct _thread_obj *_thread = obj;
    if(_thread->mode == P_THREAD_MODE_KENL || _thread->kernel_stack)
        return true;
    else
        return false;
}

void SVC_Handler(void)
{
    /**
     * r0: syscall number
     * r1-r4: syscall args(r0-r3)
     * r7: syscall api entry
     * r8: svc origin lr
     * r9: sysapi origin lr
     */
    __asm ("mov r8, lr"); /* backup lr */
    __asm ("push {r1-r9}"); /* store args(r0-r5) and r7~r9 */
    /* get syscall_api entry */
    __asm ("bl p_syscall_get_api");
    __asm ("mov r7, r0");

    __asm ("mrs r0, psp");
    __asm ("LDR     R9, [R0, #24]"); /* get user pc to r9 */
    /* todo Odd address, return to tumb mode */
    __asm ("add r9, #1");
    __asm ("STR     R9, [R0, #20]"); /* push user pc to lr */

    __asm ("mov r6, r0");
    __asm ("pop {r0-r5}"); /* restore args(r0-r5) */
      /**
     * r0-r5: syscall args
     * r6: kernel stack
     * r7: syscall api entry
     * r8: svc origin lr
     */
    __asm ("STR     R7, [R6, #24]"); /* push syscall api entry to pc */
    __asm ("STR     R0, [R6, #0]"); /* push syscall api r0 to r0 */
    __asm ("STR     R1, [R6, #4]"); /* push syscall api r1 to r1 */
    __asm ("STR     R2, [R6, #8]"); /* push syscall api r2 to r2 */
    __asm ("STR     R3, [R6, #12]"); /* push syscall api r3 to r3 */

    __asm ("mov lr, r8");  /* revert lr */
    __asm ("pop {r7-r9}"); /* restore r7~r9 */
    __asm ("    ORR     lr, lr, #0x04");
    __asm ("    BX      lr");
}

__attribute__((naked)) void arch_syscall(int sycall_no, ...)
{
    __asm ("push {r4-r7,lr}");
    
    __asm ("push {r0-r3}");
    /* syscall enter, return syscall_no */
    __asm ("bl syscall_enter");
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
    
    /* syscall enter, return syscall_no */
    __asm ("bl syscall_leave");
    __asm ("pop {r4, r5}"); /* syscall args(r0-r5) Remove the reserved parameter position */
    __asm ("pop {r4-r7,pc}");
}

/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <puppy.h>
#include <puppy/target.h>

char _kernel_stack[1024], *stack_top = _kernel_stack + 1024;

void SVC_Handler(void)
{
    /**
     * r0: syscall number
     * r1-r6: syscall args(r0-r5)
     * r7: syscall api entry
     * r8: svc orgn lr
     */
  __asm ("mov r8, lr"); /* backup lr */
  __asm ("push {r1-r6}");
  /* get syscall_api entry */
  __asm ("bl syscall_enter");
  /* get syscall_api entry */
  __asm ("bl syscall_get_api");
  __asm ("mov r7, r0");
  __asm ("pop {r0-r5}");
  /* change to kernel stack */
  __asm ("mov pc, r7");

//   __asm ("    LDR     r7, =_kernel_stack");
    
//     __asm ("    STMFD   r7!, {r1,r2,r3,r4,r5,lr,r0}");         // pop r4 - r11 register
//   __asm ("ldr sp, =stack_top");
//   __asm ("push {r1,r2,r3,r4,r5,lr,r0}");

// __asm ("    MSR     psp, r7");//  update stack pointer
__asm ("mov lr, r8"); /* restore lr */
__asm ("    ORR     lr, lr, #0x04");
__asm ("    BX      lr");
}

__attribute__((naked)) void arch_syscall(int sycall_no, ...)
{
	__asm ("push {r4-r7}");
	// __asm ("mov r7, r0");
	// __asm ("mov r0, r1");
	// __asm ("mov r1, r2");
	// __asm ("mov r2, r3");
	__asm ("add r7, sp,#16");
	__asm ("ldmia r7,{r4-r6}");
	__asm ("svc 0");
	__asm ("pop {r4-r7}");
	__asm ("bx lr");
}

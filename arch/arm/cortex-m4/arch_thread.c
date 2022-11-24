/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <puppy.h>
#include "stm32f4xx_hal.h"

extern struct _thread_obj *_g_curr_thread;
extern struct _thread_obj *_g_next_thread;

uint32_t arch_switch_interrupt_flag;

struct arch_thread
{
    uint32_t basepri;
    uint32_t stack_ptr;
};

typedef struct exc_stack_frame
{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
}_esf_t;

typedef struct stack_frame
{
    /* r4 ~ r11 register */
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;

    _esf_t esf;
}_sf_t;

void arch_new_thread(struct _thread_obj *thread,
                            void    *stack_addr,
                            uint32_t stack_size)
{
    int i;
    struct arch_thread *arch_data;
    _sf_t *sf;
    
    arch_data = (struct arch_thread *)P_ALIGN_DOWN(((uint32_t)stack_addr + stack_size) - sizeof(struct arch_thread), 8);
    sf = (_sf_t *)P_ALIGN_DOWN((uint32_t)arch_data - sizeof(_sf_t), 8);

    /* init all register */
    for (i = 0; i < sizeof(_sf_t) / sizeof(uint32_t); i ++)
    {
        ((uint32_t *)sf)[i] = 0xdeadbeef;
    }
    
    sf->esf.r0 = (uint32_t)thread->entry;
    sf->esf.r1 = (uint32_t)thread->param;
    sf->esf.r2 = 0;
    sf->esf.r3 = 0;
    sf->esf.r12 = 0;
    sf->esf.lr = 0;
    sf->esf.pc = (uint32_t)p_thread_entry;
    sf->esf.psr = 0x01000000UL;

    arch_data->stack_ptr = ((uint32_t)sf);
    thread->arch = arch_data;
}

uint32_t arch_thread_to_ptr(struct _thread_obj *thread)
{
    return (uint32_t)&thread->arch->stack_ptr;
}

void arch_swap(unsigned int key)
{
    arch_switch_interrupt_flag = 1;

	/* set pending bit to make sure we will take a PendSV exception */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

	/* clear mask or enable all irqs to take a pendsv */
	arch_irq_unlock(0);
	arch_irq_unlock(key);
}

__attribute__((always_inline)) static inline void do_swap(void)
{
    // disable interrupt to protect context switch
    __asm ("    MRS     r2, PRIMASK");
    __asm ("    CPSID   I");
    // get arch_switch_interrupt_flag
    __asm ("    LDR     r0, =arch_switch_interrupt_flag");
    __asm ("    LDR     r1, [r0]");
    __asm ("    CBZ     r1, pendsv_exit");         // pendsv already handled

    // clear arch_switch_interrupt_flag to 0
    __asm ("    MOV     r1, #0x00");
    __asm ("    STR     r1, [r0]");

    __asm ("    LDR     r0, =_g_curr_thread");
    __asm ("    LDR     r1, [r0]");
    __asm ("    CBZ     r1, switch_to_thread ");   // skip register save at the first time
    __asm ("    MOV     r0, r1");

    __asm ("    PUSH    {r0, lr}");
    __asm ("    BL      p_sched_swap_out_cb");
    __asm ("    POP     {r0, lr}");
    
    __asm ("    MRS     r1, psp              ");   // get from thread stack pointer

    __asm ("    STMFD   r1!, {r4 - r11}      ");   // push r4 - r11 register
    
    __asm ("    PUSH    {lr}");
    __asm ("    BL      arch_thread_to_ptr");
    __asm ("    POP     {lr}");
    __asm ("    STR     r1, [r0]             ");   // update from thread stack pointer

    __asm ("switch_to_thread:");
    __asm ("    LDR     r0, =_g_next_thread");
    __asm ("    LDR     r0, [r0]");
    __asm ("    PUSH    {lr}");
    __asm ("    BL      arch_thread_to_ptr");
    __asm ("    POP     {lr}");
    
    __asm ("    LDR     r1, [r0]");
    
    __asm ("    LDMFD   r1!, {r4 - r11}");         // pop r4 - r11 register
    
    __asm ("    MSR     psp, r1");//  update stack pointer
    
    __asm ("    PUSH    {r0, lr}");
    __asm ("    BL      p_sched_swap_in_cb");
    __asm ("    POP     {r0, lr}");

    __asm ("pendsv_exit:");
    // restore interrupt
    __asm ("    MSR     PRIMASK, r2");

    __asm ("    ORR     lr, lr, #0x04");
    __asm ("    BX      lr");
}

void PendSV_Handler(void)
{
    do_swap();
}
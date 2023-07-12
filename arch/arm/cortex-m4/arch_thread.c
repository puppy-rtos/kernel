/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <puppy/target.h>

extern struct _thread_obj *_g_curr_thread;
extern struct _thread_obj *_g_next_thread;

uint32_t arch_switch_interrupt_flag;
uint32_t arch_switch_interrupt_from_sp;
uint32_t arch_switch_interrupt_to_sp;

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

void arch_swap(unsigned int key)
{
    arch_switch_interrupt_flag = 1;

	/* set pending bit to make sure we will take a PendSV exception */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    if (p_cpu_self()->curr_thread)
    {
        arch_switch_interrupt_from_sp = &p_cpu_self()->curr_thread->arch->stack_ptr;
    }
    if (p_cpu_self()->next_thread)
    {
        arch_switch_interrupt_to_sp = &p_cpu_self()->next_thread->arch->stack_ptr;
    }

	/* clear mask or enable all irqs to take a pendsv */
	arch_irq_unlock(0);
	arch_irq_unlock(key);
}
__attribute__((naked)) void PendSV_Handler(void)
{
#ifndef P_ARCH_CORTEX_M0
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

    __asm ("    LDR     r0, =arch_switch_interrupt_from_sp");
    __asm ("    LDR     r1, [r0]");
    __asm ("    CBZ     r1, switch_to_thread ");   // skip register save at the first time
    __asm ("    MOV     r0, r1");

    __asm ("    PUSH    {r0, lr}");
    __asm ("    BL      p_sched_swap_out_cb");
    __asm ("    POP     {r0, lr}");
    
    __asm ("    MRS     r1, psp              ");   // get from thread stack pointer

    __asm ("    STMFD   r1!, {r4 - r11}      ");   // push r4 - r11 register
    __asm ("    STR     r1, [r0]             ");   // update from thread stack pointer

    __asm ("switch_to_thread:");
    __asm ("    LDR     r0, =arch_switch_interrupt_to_sp");
    __asm ("    LDR     r1, [r0]");
    __asm ("    LDR     r1, [r1]");
    
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
#else
    // disable interrupt to protect context switch
    __asm ("  .cpu    cortex-m0 ");
    __asm ("  .fpu    softvfp");
    __asm ("  .syntax unified");
    __asm ("  .thumb");
    
    __asm ("    MRS     r2, PRIMASK");
    __asm ("    CPSID   I");
    // get arch_switch_interrupt_flag
    __asm ("    LDR     r0, =arch_switch_interrupt_flag");
    __asm ("    LDR     r1, [r0]");
    __asm ("    CMP     r1, #0x00");
    __asm ("    BEQ     pendsv_exit");
    // clear arch_switch_interrupt_flag to 0
    __asm ("    MOVS     r1, #0");
    __asm ("    STR     r1, [r0]");

    __asm ("    LDR     r0, =arch_switch_interrupt_from_sp");
    __asm ("    LDR     r1, [r0]");
    __asm ("    CMP     r1, #0x00");
    __asm ("    BEQ     switch_to_thread");
    __asm ("    MOV     r0, r1");

    __asm ("    PUSH    {lr}");
    __asm ("    PUSH    {r0}");
    __asm ("    BL      p_sched_swap_out_cb");
    __asm ("    POP     {r0}");
    __asm ("    POP     {r3}");
    
    __asm ("    MRS     r1, psp              ");   // get from thread stack pointer
    __asm ("    SUBS    R1, R1, #0x20       ");     /* space for {R4 - R7} and {R8 - R11} */
    __asm ("    STR     r1, [r0]             ");   // update from thread stack pointer
    __asm ("    STMIA   R1!, {R4 - R7}      ");     /* push thread {R4 - R7} register to thread stack */
    __asm ("    MOV     R4, R8              ");     /* mov thread {R8 - R11} to {R4 - R7} */
    __asm ("    MOV     R5, R9");  
    __asm ("    MOV     R6, R10");  
    __asm ("    MOV     R7, R11");  
    __asm ("    STMIA   R1!, {R4 - R7}      ");     /* push thread {R8 - R11} high register to thread stack */

    __asm ("switch_to_thread:");
    __asm ("    LDR     r0, =arch_switch_interrupt_to_sp");
    __asm ("    LDR     r0, [r0]");
    __asm ("    LDR     r1, [r0]");
    __asm ("   LDMIA   R1!, {R4 - R7} ");        /* pop thread {R4 - R7} register from thread stack */
    __asm ("   PUSH    {R4 - R7}      ");        /* push {R4 - R7} to MSP for copy {R8 - R11} */
    __asm ("   LDMIA   R1!, {R4 - R7} ");        /* pop thread {R8 - R11} high register from thread stack to {R4 - R7} */
    __asm ("   MOV     R8,  R4        ");        /* mov {R4 - R7} to {R8 - R11} */
    __asm ("   MOV     R9,  R5");
    __asm ("   MOV     R10, R6");
    __asm ("   MOV     R11, R7");
    __asm ("   POP     {R4 - R7}      ");        /* pop {R4 - R7} from MSP */

    __asm ("    MSR     psp, r1");//  update stack pointer
    
    __asm ("    PUSH    {r3}");
    __asm ("    PUSH    {r0}");
    __asm ("    BL      p_sched_swap_in_cb");
    __asm ("    POP     {r0}");
    __asm ("    POP     {r3}");

    __asm ("pendsv_exit:");
    // restore interrupt
    __asm ("    MSR     PRIMASK, r2");

    __asm ("    MOVS    R3, #0x03");
    __asm ("    RSBS    R3, R3, #0x00");
    __asm ("    BX      r3");
#endif
}

struct exception_info
{
    uint32_t exc_return;
    struct stack_frame stack_frame;
};

void dump_contex_esf(_esf_t *esf)
{
    printk("psr: 0x%08x\n", esf->psr);
    printk("r00: 0x%08x\n", esf->r0);
    printk("r01: 0x%08x\n", esf->r1);
    printk("r02: 0x%08x\n", esf->r2);
    printk("r03: 0x%08x\n", esf->r3);
    printk("r12: 0x%08x\n", esf->r12);
    printk(" lr: 0x%08x\n", esf->lr);
    printk(" pc: 0x%08x\n", esf->pc);
}

void dump_contex(struct stack_frame *context)
{
    printk("psr: 0x%08x\n", context->esf.psr);
    printk("r00: 0x%08x\n", context->esf.r0);
    printk("r01: 0x%08x\n", context->esf.r1);
    printk("r02: 0x%08x\n", context->esf.r2);
    printk("r03: 0x%08x\n", context->esf.r3);
    printk("r04: 0x%08x\n", context->r4);
    printk("r05: 0x%08x\n", context->r5);
    printk("r06: 0x%08x\n", context->r6);
    printk("r07: 0x%08x\n", context->r7);
    printk("r08: 0x%08x\n", context->r8);
    printk("r09: 0x%08x\n", context->r9);
    printk("r10: 0x%08x\n", context->r10);
    printk("r11: 0x%08x\n", context->r11);
    printk("r12: 0x%08x\n", context->esf.r12);
    printk(" lr: 0x%08x\n", context->esf.lr);
    printk(" pc: 0x%08x\n", context->esf.pc);
}

void arch_hardfault_exception(struct exception_info *exception_info)
{
    struct stack_frame *context = &exception_info->stack_frame;

    if (exception_info->exc_return & (1 << 2))
    {
        printk("hard fault on thread: %s\n", p_thread_self_name());
    }
    else
    {
        printk("hard fault on handler\n");
    }

    if ( (exception_info->exc_return & 0x10) == 0)
    {
        printk("FPU active!\n");
    }

    dump_contex(context);
    list_thread();

    while (1);
}

__attribute__((naked)) void HardFault_Handler(void)
{
#ifndef P_ARCH_CORTEX_M0
    // get current context
    __asm ("    TST     lr, #0x04");
    __asm ("    ITE     EQ");
    __asm ("    MRSEQ   r0, msp");
    __asm ("    MRSNE   r0, psp");
    __asm ("    STMFD   r0!, {r4 - r11}      ");   // push r4 - r11 register
    __asm ("    STMFD   r0!, {lr}      ");   // push r4 - r11 register

    __asm ("    TST     lr, #0x04");
    __asm ("    ITE     EQ");
    __asm ("    MSREQ   msp, r0");
    __asm ("    MSRNE   psp, r0");

    __asm ("    PUSH    {lr}");
    __asm ("    BL      arch_hardfault_exception");
    __asm ("    POP     {lr}");

    __asm ("    ORR     lr, lr, #0x04");
    __asm ("    BX      lr");
#else
    __asm ("    MRS     R0, PSP                 ");
    __asm ("    PUSH    {LR}");
    __asm ("    BL      arch_hardfault_exception");
    __asm ("    POP     {PC}");
#endif
}

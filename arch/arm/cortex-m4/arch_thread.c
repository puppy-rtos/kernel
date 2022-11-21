/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <puppy.h>
#include "stm32f4xx_hal.h"

extern struct _thread_obj *_g_curr_thread;
extern struct _thread_obj *_g_next_thread;
static struct arch_thread
{
    uint32_t basepri;
    uint32_t stack_ptr;
}_arch_thread_data;

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


void arch_new_thread(struct _thread_obj *thread,
                            void    *stack_addr,
                            uint32_t stack_size)
{
    _esf_t *esf;
    esf = (_esf_t *)P_ALIGN_DOWN(((uint32_t)stack_addr + stack_size) - sizeof(_esf_t), 8);

    esf->r0 = (uint32_t)thread->entry;
    esf->r1 = (uint32_t)thread->param;
    esf->r2 = 0;
    esf->r3 = 0;
    esf->r12 = 0;
    esf->lr = 0;
    esf->pc = (uint32_t)p_thread_entry;
    esf->psr = 0x01000000UL;

    _arch_thread_data.stack_ptr = ((uint32_t)esf);
    thread->arch = &_arch_thread_data;
}

 uint32_t arch_thread_to_ptr(struct _thread_obj *thread)
{
    return thread->arch->stack_ptr;
}

void arch_swap(unsigned int key)
{
	/* store off key and return value */
    if (_g_curr_thread)
    {
        _g_curr_thread->arch->basepri = key;
    }

	/* set pending bit to make sure we will take a PendSV exception */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;

	/* clear mask or enable all irqs to take a pendsv */
	arch_irq_unlock(0);
}
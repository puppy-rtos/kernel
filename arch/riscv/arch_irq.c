/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

__attribute__((always_inline)) inline p_base_t arch_irq_lock(void)
{
    p_base_t key;

    __asm volatile("csrrci %0, mstatus, 8"
        : "=r" (key)
        :
        : "memory");

    return key;
}

__attribute__((always_inline)) inline void arch_irq_unlock(p_base_t key)
{
    __asm volatile(
        "csrw mstatus, %1;"
        : "=r" (key)
        : "r" (key)
        : "memory");
}

__attribute__((always_inline)) inline bool arch_irq_locked(p_base_t key)
{
    /* todo */
    return key != 0U;
}
__attribute__((always_inline)) inline bool arch_in_irq(void)
{
    // volatile int tmp = 0;
    // __asm volatile("mrs %0, IPSR;"
    //     : "=r" (tmp)
    //     :
    //     : "memory");
    // return (tmp & 0x1f) != 0U;      

}
#include "platform.h"
#include "riscv.h"
extern void trap_vector(void);
extern void uart_isr(void);
extern void timer_handler(void);

void trap_init()
{
	/*
	 * set the trap-vector base-address for machine-mode
	 */
	w_mtvec((uint32_t)trap_vector);
}

void external_interrupt_handler()
{
	int irq = plic_claim();

	if (irq == 10){
      		uart_isr();
	} else if (irq) {
		printk("unexpected interrupt irq = %d\n", irq);
	}
	
	if (irq) {
		plic_complete(irq);
	}
}

uint32_t trap_handler(uint32_t epc, uint32_t cause)
{
	uint32_t return_pc = epc;
	uint32_t cause_code = cause & 0xfff;
	
	if (cause & 0x80000000) {
		/* Asynchronous trap - interrupt */
		switch (cause_code) {
		case 3:
			printk("software interruption!\n");
			break;
		case 7:
			// printk("timer interruption!\n");
			timer_handler();
			break;
		case 11:
			// printk("external interruption!\n");
			external_interrupt_handler();
			break;
		default:
			printk("unknown async exception!\n");
			break;
		}
	} else {
		/* Synchronous trap - exception */
		printk("Sync exceptions!, code = %d\n", cause_code);
		printk("OOPS! What can I do!");
        while(1)
        {}
		// return_pc += 2;
	}

	return return_pc;
}

void trap_test()
{
	/*
	 * Synchronous exception code = 7
	 * Store/AMO access fault
	 */
	*(int *)0x00000000 = 100;

	/*
	 * Synchronous exception code = 5
	 * Load access fault
	 */
	//int a = *(int *)0x00000000;

	printk("Yeah! I'm return back from trap!\n");
}
#include "nr_micro_shell.h"
NR_SHELL_CMD_EXPORT(trap_test, trap_test);

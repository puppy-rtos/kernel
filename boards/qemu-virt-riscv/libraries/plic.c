#include <puppy_core.h>

#include <stdint.h>
#include <stddef.h>

#include "platform.h"
#include "riscv.h"

#define UART0_IRQ 10
void plic_init(void)
{
	int hart = r_mhartid();
  
	/* 
	 * Set priority for UART0.
	 *
	 * Each PLIC interrupt source can be assigned a priority by writing 
	 * to its 32-bit memory-mapped priority register.
	 * The QEMU-virt (the same as FU540-C000) supports 7 levels of priority. 
	 * A priority value of 0 is reserved to mean "never interrupt" and 
	 * effectively disables the interrupt. 
	 * Priority 1 is the lowest active priority, and priority 7 is the highest. 
	 * Ties between global interrupts of the same priority are broken by 
	 * the Interrupt ID; interrupts with the lowest ID have the highest 
	 * effective priority.
	 */
	*(uint32_t*)PLIC_PRIORITY(UART0_IRQ) = 1;
 
	/*
	 * Enable UART0
	 *
	 * Each global interrupt can be enabled by setting the corresponding 
	 * bit in the enables registers.
	 */
	*(uint32_t*)PLIC_MENABLE(hart)= (1 << UART0_IRQ);

	/* 
	 * Set priority threshold for UART0.
	 *
	 * PLIC will mask all interrupts of a priority less than or equal to threshold.
	 * Maximum threshold is 7.
	 * For example, a threshold value of zero permits all interrupts with
	 * non-zero priority, whereas a value of 7 masks all interrupts.
	 * Notice, the threshold is global for PLIC, not for each interrupt source.
	 */
	*(uint32_t*)PLIC_MTHRESHOLD(hart) = 0;

	/* enable machine-mode external interrupts. */
	w_mie(r_mie() | MIE_MEIE);

	/* enable machine-mode global interrupts. */
	// w_mstatus(r_mstatus() | MSTATUS_MIE);
}

/* 
 * DESCRIPTION:
 *	Query the PLIC what interrupt we should serve.
 *	Perform an interrupt claim by reading the claim register, which
 *	returns the ID of the highest-priority pending interrupt or zero if there 
 *	is no pending interrupt. 
 *	A successful claim also atomically clears the corresponding pending bit
 *	on the interrupt source.
 * RETURN VALUE:
 *	the ID of the highest-priority pending interrupt or zero if there 
 *	is no pending interrupt.
 */
int plic_claim(void)
{
	int hart = r_mhartid();
	int irq = *(uint32_t*)PLIC_MCLAIM(hart);
	return irq;
}

/* 
 * DESCRIPTION:
  *	Writing the interrupt ID it received from the claim (irq) to the 
 *	complete register would signal the PLIC we've served this IRQ. 
 *	The PLIC does not check whether the completion ID is the same as the 
 *	last claim ID for that target. If the completion ID does not match an 
 *	interrupt source that is currently enabled for the target, the completion
 *	is silently ignored.
 * RETURN VALUE: none
 */
void plic_complete(int irq)
{
	int hart = r_mhartid();
	*(uint32_t*)PLIC_MCOMPLETE(hart) = irq;
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
		PUP_PRINTK("unexpected interrupt irq = %d\n", irq);
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
			// PUP_PRINTK("software interruption!\n");
			sfi_handler();
			break;
		case 7:
			// PUP_PRINTK("timer interruption!\n");
			timer_handler();
			break;
		case 11:
			// PUP_PRINTK("external interruption!\n");
			external_interrupt_handler();
			break;
		default:
			PUP_PRINTK("unknown async exception!\n");
			break;
		}
	} else {
		/* Synchronous trap - exception */
		PUP_PRINTK("Sync exceptions!, code = %d\n", cause_code);
		PUP_PRINTK("OOPS! What can I do!");
		// list_thread();
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

	PUP_PRINTK("Yeah! I'm return back from trap!\n");
}
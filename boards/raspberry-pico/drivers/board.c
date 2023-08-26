/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author         Notes
 * 2021-01-28     flybreak       first version
 * 2023-01-22     rose_man       add RT_USING_SMP
 */


#include <stdio.h>
#include <puppy.h>
#include "board.h"

#include "hardware/structs/systick.h"
#include "hardware/structs/sio.h"

#include "hardware/uart.h"
#include "hardware/irq.h"
#include <pico/lock_core.h>

#define KLOG_TAG  "board"
#define KLOG_LVL   KLOG_LOG
#include <puppy/klog.h>

#define PLL_SYS_KHZ (133 * 1000)
#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE
#define UART_TX_PIN 0
#define UART_RX_PIN 1

p_rb_t cons_rb;
char buf[128];
static struct _sem_obj cons_sem;
int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

int arch_uart_init(void)
{

    uart_init(UART_ID, 115200);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Actually, we want a different speed
    // The call will return the actual baud rate selected, which will be as close as
    // possible to that requested
    uart_set_baudrate(UART_ID, BAUD_RATE);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);

        /* set the PendSV and SysTick exception priority */
    __asm ("  .cpu    cortex-m0 ");
    __asm ("  .fpu    softvfp");
    __asm ("  .syntax unified");
    __asm ("  .thumb");
    
    __asm ("  .equ    NVIC_SHPR3, 0xE000ED20     ");     /* system priority register (3) */
    __asm ("  .equ    NVIC_PENDSV_PRI, 0xFFFF0000");     /* PendSV and SysTick priority value (lowest) */
    __asm ("   LDR     R0, =NVIC_SHPR3");
    __asm ("   LDR     R1, =NVIC_PENDSV_PRI");
    __asm ("   LDR     R2, [R0,#0x00]       ");/* read */
    __asm ("   ORRS    R1, R1, R2           ");  /* modify */
    __asm ("   STR     R1, [R0]             ");/* write-back */

    return 0;
}
void pico_uart_isr(void)
{
    /* read interrupt status and clear it */
    uint8_t ch;
    if (uart_is_readable(uart0))
    {
        ch = uart_get_hw(uart0)->dr & 0xFF;
        p_rb_write(&cons_rb, &ch, 1);
        p_sem_post(&cons_sem);
    }
}

int _cons_init(void)
{
    static int _inited = 0;
    if (!_inited)
    {
        _inited = 1;
        
        p_sem_init(&cons_sem, "cons_sem", 0, 1);
        p_rb_init(&cons_rb, buf, 128);
        /* enable interrupt */
        // Set up a RX interrupt
        // We need to set up the handler first
        // And set up and enable the interrupt handlers
        irq_set_exclusive_handler(UART_IRQ, pico_uart_isr);
        irq_set_enabled(UART_IRQ, true);

        // Now enable the UART to send interrupts - RX only
        uart_set_irq_enables(UART_ID, true, false);
    }
    return 0;
}

int p_hw_cons_getc(void)
{
    uint8_t ch = -1;
    _cons_init();

__retry:
    if (p_rb_read(&cons_rb, &ch, 1) == false)
    {
        p_sem_wait(&cons_sem);
        goto __retry;
    }
    return ch;
}

int p_hw_cons_output(const char *str, int len)
{
    int i = 0;

    for(i = 0; i < len; i++)
    {
        if (str[i] == '\n')
        {
            char n = '\r';
            uart_putc_raw(uart0, n);
        }
        uart_putc_raw(uart0, str[i]);
    }
    return 0;
}
void isr_systick(void)
{
    p_tick_inc(1, (uint32_t)mpu_hw->rvr - (uint32_t)mpu_hw->cvr);
}

#if P_CPU_NR > 1

#include <pico/multicore.h>

#define IPI_MAGIC 0x5a5a
uint8_t p_cpu_self_id()
{
    return sio_hw->cpuid;   
}
void _hw_ipi_handler(void)
{
    uint32_t status = sio_hw->fifo_st;
    multicore_fifo_clear_irq();

    if ( status & SIO_FIFO_ST_VLD_BITS )
    {
        if ( sio_hw->fifo_rd == IPI_MAGIC )
        {
            p_sched();
        }
    }
    multicore_fifo_drain();
}
void isr_irq15(void)
{
    _hw_ipi_handler();
}

void isr_irq16(void)
{
    _hw_ipi_handler();
}

void arch_ipi_send(uint8_t cpuid)
{
    multicore_fifo_push_blocking(IPI_MAGIC);
}

#include "nr_micro_shell.h"
void shell_ipi_cmd(char argc, char *argv)
{
    arch_ipi_send(0);
}
NR_SHELL_CMD_EXPORT(ipi, shell_ipi_cmd);

void _multcore_entry()
{
    /* Install FIFO handler to receive interrupt from other core */
    multicore_fifo_clear_irq();
    multicore_fifo_drain();
    irq_set_priority(SIO_IRQ_PROC1, 255UL);
    irq_set_exclusive_handler(SIO_IRQ_PROC1, isr_irq16 );
    irq_set_enabled(SIO_IRQ_PROC1, 1);
    
    puppy_start();
}

void p_subcpu_start(void)
{
    multicore_launch_core1(_multcore_entry);
    /* Install FIFO handler to receive interrupt from other core */
    multicore_fifo_clear_irq();
    multicore_fifo_drain();
    irq_set_priority(SIO_IRQ_PROC0, 255UL);
    irq_set_exclusive_handler( SIO_IRQ_PROC0, isr_irq15);
    irq_set_enabled(SIO_IRQ_PROC0, 1 );
}
#endif

uint32_t systick_config(uint32_t ticks)
{
  if ((ticks - 1UL) > M0PLUS_SYST_RVR_RELOAD_BITS)
  {
    return (1UL);                                                   /* Reload value impossible */
  }

  mpu_hw->rvr    = (uint32_t)(ticks - 1UL);                         /* set reload register */
  mpu_hw->csr  = M0PLUS_SYST_CSR_CLKSOURCE_BITS |
                   M0PLUS_SYST_CSR_TICKINT_BITS   |
                   M0PLUS_SYST_CSR_ENABLE_BITS;                     /* Enable SysTick IRQ and SysTick Timer */
  return (0UL);                                                     /* Function successful */
}
char heap_buf[50*1024];

int puppy_board_init(void)
{
    set_sys_clock_khz(PLL_SYS_KHZ, true);

    p_tick_init(100, clock_get_hz(clk_sys)/1000000);
    arch_spin_lock_init(&cons_lock);
    p_system_heap_init(heap_buf, sizeof(heap_buf));


    alarm_pool_init_default();

    // Start and end points of the constructor list,
    // defined by the linker script.
    extern void (*__init_array_start)();
    extern void (*__init_array_end)();

    /* Configure the SysTick */
    systick_config(clock_get_hz(clk_sys)/100);

    stdio_init_all();
    arch_uart_init();
    
    // Call each function in the list.
    // We have to take the address of the symbols, as __init_array_start *is*
    // the first function pointer, not the address of it.
    for (void (**p)() = &__init_array_start; p < &__init_array_end; ++p) {
        (*p)();
    }

    return 0;
}

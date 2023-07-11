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

#include "hardware/uart.h"
#include "hardware/irq.h"

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

int rt_hw_uart_init(void)
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
    // rt_interrupt_enter();
    /* read interrupt status and clear it */
    char ch = -1;
    if (uart_is_readable(uart0))
    {
        ch = uart_get_hw(uart0)->dr;
        p_rb_write(&cons_rb, &ch, 1);
        p_sem_post(&cons_sem);
    }
    // rt_interrupt_leave();
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
            // HAL_UART_Transmit(&huart1, (uint8_t *)&n, 1, 0xffff);
        }
        uart_putc_raw(uart0, str[i]);
        // HAL_UART_Transmit(&huart1, (uint8_t *)&str[i], 1, 0xFFFF);
    }
    return 0;
}
void isr_systick(void)
{
    /* enter interrupt */
// #ifndef RT_USING_SMP
//     rt_interrupt_enter();
// #endif

    p_tick_inc();

    /* leave interrupt */
// #ifndef RT_USING_SMP
//     rt_interrupt_leave();
// #endif
}

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

void rt_hw_board_init()
{
    set_sys_clock_khz(PLL_SYS_KHZ, true);

    p_tick_init(100);
    p_system_heap_init(heap_buf, sizeof(heap_buf));
// #ifdef RT_USING_HEAP
//     rt_system_heap_init(HEAP_BEGIN, HEAP_END);
// #endif

// #ifdef RT_USING_SMP
//     extern rt_hw_spinlock_t _cpus_lock;
//     rt_hw_spin_lock_init(&_cpus_lock);
// #endif

    alarm_pool_init_default();

    // Start and end points of the constructor list,
    // defined by the linker script.
    extern void (*__init_array_start)();
    extern void (*__init_array_end)();

    // Call each function in the list.
    // We have to take the address of the symbols, as __init_array_start *is*
    // the first function pointer, not the address of it.
    for (void (**p)() = &__init_array_start; p < &__init_array_end; ++p) {
        (*p)();
    }

    /* Configure the SysTick */
    systick_config(clock_get_hz(clk_sys)/100);

// #ifdef RT_USING_COMPONENTS_INIT
//     rt_components_board_init();
// #endif

// #ifdef RT_USING_SERIAL
    stdio_init_all();
    rt_hw_uart_init();
// #endif

// #ifdef RT_USING_CONSOLE
//    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
// #endif
}

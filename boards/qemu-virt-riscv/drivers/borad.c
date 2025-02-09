
#include <puppy_core.h>
#include "drv_uart.h"

#include <platform.h>

// #define KLOG_TAG  "drv.board"
// #define KLOG_LVL   KLOG_INFO
// #include <puppy_klog.h>

/* USER CODE BEGIN PFP */
// char heap_buf[50*1024];
// pup_rb_t cons_rb;
// char buf[128];
// static struct _sem_obj cons_sem;

int _cons_init(void)
{
    static int _inited = 0;
    if (!_inited)
    {
        _inited = 1;
        // pup_rb_init(&cons_rb, buf, 128);
        uart_init();
    }
}

// int pup_hw_cons_getc(void)
// {
//     int ch = -1;
//     _cons_init();

// __retry:
//     if (pup_rb_read(&cons_rb, &ch, 1) == false)
//     {
//         pup_sem_wait(&cons_sem);
//         goto __retry;
//     }
//     return ch;
// }

int pup_hw_cons_output(const char *str, int len)
{
    size_t i;
    _cons_init();

    for (i = 0; i < len; i++) {
        if (str[i] == '\n')
        {
            uart_putc('\r');
        }
        uart_putc(str[i]);
    }
    return 0;
}
void uart_isr(void)
{
    int ch = -1;
    // ch = uart_getc();
    // if (ch != -1)
    // {
    //     pup_rb_write(&cons_rb, &ch, 1);
    //     pup_sem_post(&cons_sem);
    // }
}

int puppy_board_init(void)
{
    arch_irq_lock();
    trap_init();
    plic_init();
    timer_init();
    
    // pup_tick_init(100, CLINT_TIMEBASE_FREQ / 1000000);
    // pup_sem_init(&cons_sem, "cons_sem", 0, 1);
    // pup_system_heap_init(heap_buf, sizeof(heap_buf));

    return 0;
}

/**
 * smp support
*/
#if PUP_CPU_NR > 1
#include <platform.h>
#include <riscv.h>

volatile pup_base_t _g_subcpu_start_flag = 0;

uint8_t pup_cpu_self_id()
{
    return r_mhartid();
}

void subcpu_entry(void)
{
    while(!_g_subcpu_start_flag);
    // KLOG_I("I am core %d!", pup_cpu_self_id());
    arch_irq_lock();
    trap_init();
    plic_init();
    w_mie(r_mie() | MIE_MSIE);
    pup_sched_unlock();
    while(1);
}

void sfi_handler(void)
{
    *(uint32_t*)CLINT_MSIP(pup_cpu_self_id()) = 0;
    pup_sched();
}

void arch_irq_enable()
{
    w_mstatus(r_mstatus() | MSTATUS_MIE);
}
void arch_ipi_send(uint8_t cpuid)
{
    *(uint32_t*)CLINT_MSIP(cpuid) = 1;
}

void pup_subcpu_start(void)
{
    w_mie(r_mie() | MIE_MSIE);
    _g_subcpu_start_flag = 1;
}
#else
void sfi_handler(void)
{

}
void subcpu_entry(void)
{
    while(1);
}
#endif

#include <puppy.h>
#include "riscv-virt.h"
#include "drv_uart.h"

/* USER CODE BEGIN PFP */
char heap_buf[50*1024];
p_rb_t cons_rb;
char buf[128];
static struct _sem_obj cons_sem;

int _cons_init(void)
{
    static int _inited = 0;
    if (!_inited)
    {
        _inited = 1;
        p_rb_init(&cons_rb, buf, 128);
        uart_init();
    }
}

int p_hw_cons_getc(void)
{
    int ch = -1;
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
    size_t i;

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
    ch = uart_getc();
    if (ch != -1)
    {
        p_rb_write(&cons_rb, &ch, 1);
        p_sem_post(&cons_sem);
    }
}

int board_init(void)
{
    arch_irq_lock();
    plic_init();
    timer_init();
    p_tick_init(100);
    p_sem_init(&cons_sem, "cons_sem", 0, 1);
    p_system_heap_init(heap_buf, sizeof(heap_buf));

    return 0;
}

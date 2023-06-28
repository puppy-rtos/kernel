/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "main.h"
#include <puppy.h>

#ifdef ENABLE_NR_SHELL
#include "nr_micro_shell.h"

struct _thread_obj shell;
__attribute__((aligned(4)))
uint8_t shell_thread_stack[1024];

void shell_thread_entry(void *parm)
{
    shell_init();
    while(1)
    {
        char c = p_hw_cons_getc();
        if(c)
        {
            shell(c);
        }
    }
}
#endif

int main(void)
{
    int board_init(void);
    board_init();
    
    printk("Hello Puppy!\r\n");
    puppy_init();
#ifdef ENABLE_NR_SHELL
    p_thread_init(&shell, "shell", shell_thread_entry, NULL,
                  shell_thread_stack,
                  sizeof(shell_thread_stack),
                  12);
    p_thread_start(&shell);
#endif
    puppy_start();
}


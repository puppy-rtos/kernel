/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

static struct _thread_obj _idle;
__attribute__((aligned(P_ALIGN_SIZE)))
uint8_t _idle_thread_stack[P_IDLE_THREAD_STACK_SIZE];

static struct _thread_obj _main;
__attribute__((aligned(P_ALIGN_SIZE)))
uint8_t _main_thread_stack[P_MAIN_THREAD_STACK_SIZE];

void idle_thread_entry(void *parm)
{
    while(1)
    {

    }
}

#ifdef __ARMCC_VERSION
extern int $Super$$main(void);
/* re-define main function */
int $Sub$$main(void)
{
    puppy_init();
    return 0;
}
#endif

void main_thread_entry(void *parm)
{
    p_thread_init(&_idle, "idle", idle_thread_entry, NULL,
                  _idle_thread_stack,
                  sizeof(_idle_thread_stack),
                  P_THREAD_PRIO_MAX);
    p_thread_start(&_idle);
#ifdef __ARMCC_VERSION
    $Super$$main(); /* for ARMCC. */
#endif
}

void puppy_init(void)
{
    /*  */
    p_thread_init(&_main, "main", main_thread_entry, NULL,
                  _main_thread_stack,
                  sizeof(_main_thread_stack), 1);
    p_thread_start(&_main);
}

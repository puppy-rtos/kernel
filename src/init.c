/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <puppy/target.h>

#define KLOG_TAG  "init"
#define KLOG_LVL   KLOG_WARNING
#include <puppy/klog.h>

static struct _thread_obj _idle;
__attribute__((aligned(P_ALIGN_SIZE)))
uint8_t _idle_thread_stack[P_IDLE_THREAD_STACK_SIZE];

static struct _thread_obj _main;
__attribute__((aligned(P_ALIGN_SIZE)))
uint8_t _main_thread_stack[P_MAIN_THREAD_STACK_SIZE];

typedef void (*p_func)(void);
p_func return_to_main;

void idle_thread_entry(void *parm)
{
    while(1)
    {

    }
}

void main_thread_entry(void *parm)
{
    KLOG_D("main_thread_entry...");
    p_thread_init(&_idle, "idle", idle_thread_entry, NULL,
                  _idle_thread_stack,
                  sizeof(_idle_thread_stack),
                  P_THREAD_PRIO_MAX);
    p_thread_start(&_idle);

    return_to_main();
}

P_WEAK int p_hw_borad_init(void)
{
    return 0;
}

P_WEAK int main(void)
{
    p_hw_borad_init();
    puppy_start();
    p_thread_abort(p_thread_self());
    return 0;
}

void puppy_start(void)
{
    ARCH_GET_LR(return_to_main);
    p_thread_init(&_main, "main", main_thread_entry, NULL,
                  _main_thread_stack,
                  sizeof(_main_thread_stack), 1);
    p_thread_start(&_main);
}

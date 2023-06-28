/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

#define KLOG_TAG  "init"
#define KLOG_LVL   KLOG_WARNING
#include <puppy/klog.h>

static struct _thread_obj _idle;
__attribute__((aligned(P_ALIGN_SIZE)))
uint8_t _idle_thread_stack[P_IDLE_THREAD_STACK_SIZE];

void idle_thread_entry(void *parm)
{
    while(1)
    {

    }
}

void puppy_init(void)
{
    p_thread_init(&_idle, "idle", idle_thread_entry, NULL,
                  _idle_thread_stack,
                  sizeof(_idle_thread_stack),
                  P_THREAD_PRIO_MAX);
    p_thread_start(&_idle);
}

void puppy_start(void)
{
    p_sched_unlock();
}

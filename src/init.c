/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

#define KLOG_TAG  "init"
#define KLOG_LVL   KLOG_WARNING
#include <puppy/klog.h>

static struct _thread_obj _idle;
p_align(P_ALIGN_SIZE)
static uint8_t _idle_thread_stack[P_IDLE_THREAD_STACK_SIZE];

static void idle_thread_entry(void *parm)
{
    while(1)
    {

    }
}

P_SECTION_START_DEFINE(P_THREAD_DEFINE_SECT, _thread_start);
P_SECTION_END_DEFINE(P_THREAD_DEFINE_SECT, _thread_end);
static void _dthread_obj_init(void)
{
    unsigned int *ptr_begin, *ptr_end;
    ptr_begin = (unsigned int *)P_SECTION_START_ADDR(_thread_start);
    ptr_end = (unsigned int *)P_SECTION_END_ADDR(_thread_end);
    for (unsigned int *ptr = ptr_begin; ptr < ptr_end;)
    {
        struct _thread_obj *_obj = ptr;
        KLOG_D("dthread [%s] init...",  _obj->kobj.name);
        p_thread_init(_obj, _obj->kobj.name, _obj->entry, _obj->param,
                    _obj->stack_addr,
                    _obj->stack_size,
                    _obj->prio);
        p_thread_start(_obj);
        ptr += (sizeof(struct _thread_obj) / sizeof(unsigned int));
    }
}

void p_show_version(void)
{
    printk("Build Time: %s %s\n", __DATE__, __TIME__);
    printk("                           _         \n");
    printk("    ____   ____    _____  (_) _  __\n");
    printk("   / __ \\ / __ \\  / ___/ / / | |/_/\n");
    printk("  / /_/ // /_/ / (__  ) / /  >  <  \n");
    printk(" / .___/ \\____/ /____/ /_/  /_/|_|  \n");
    printk("/_/          Powered dy puppy-rtos\n");
}

void puppy_init(void)
{
    p_show_version();
    _dthread_obj_init();
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

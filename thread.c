/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <puppy.h>

#define P_THREAD_SLICE_DEFAULT 10

extern struct _thread_obj *_g_curr_thread;

void p_thread_entry(void (*entry)(void *parameter), void *param)
{
    if (entry)
    {
        entry(param);
    }
    printk("_thread exit!_\r\n");
    while (1);
}

void _thread_init(p_obj_t obj, const char *name,
                                void (*entry)(void *param),
                                void    *param,
                                void    *stack_addr,
                                uint32_t stack_size,
                                uint8_t  prio)
{
    struct _thread_obj *thread = obj;

    p_obj_init(&thread->kobj, name, P_OBJ_TYPE_THREAD | P_OBJ_TYPE_STATIC);
    
    thread->entry = entry;
    thread->param = param;
    thread->stack_addr = stack_addr;
    thread->stack_size = stack_size;
    thread->prio = prio;
    thread->slice_ticks = P_THREAD_SLICE_DEFAULT;
    thread->state = P_THREAD_STATE_INIT;

    arch_new_thread(thread, stack_addr, stack_size);
}
int p_thread_yield(void)
{
    struct _thread_obj *_thread = _g_curr_thread;
    p_base_t key = arch_irq_lock();

    p_sched_remove(_thread);
    p_sched_insert(_thread);
    p_sched();
    
    arch_irq_unlock(key);
}

int p_thread_start(p_obj_t obj)
{
    // struct _thread_obj *thread = obj;
    
    p_sched_insert(obj);
    
    p_sched();
    return 0;
}

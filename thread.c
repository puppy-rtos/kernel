/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <puppy.h>

#define P_THREAD_SLICE_DEFAULT 10

struct _thread_obj tlist[3];

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


int p_thread_start(p_obj_t obj)
{
    // struct _thread_obj *thread = obj;
    
    p_schedule_insert(obj);
    return 0;
}

void thread_init_tc(void)
{
    //_thread_init();
}

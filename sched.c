/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

p_list_t ready_queue = P_LIST_STATIC_INIT(&ready_queue);
struct _thread_obj *_g_curr_thread;
struct _thread_obj *_g_next_thread;

int p_schedule(void)
{
    p_base_t key = arch_irq_lock();

    if (!_g_next_thread)
    {
        _g_next_thread = p_list_entry(ready_queue.head, struct _thread_obj, tnode);
        p_list_remove(ready_queue.head);
    }
    _g_next_thread->state = P_THREAD_STATE_RUN;

    arch_swap();

    arch_irq_unlock(key);
    return -1;
}

int p_schedule_insert(p_obj_t thread)
{
    struct _thread_obj *_thread = thread;
    p_base_t key = arch_irq_lock();

    _thread->state = P_THREAD_STATE_READY;
    p_list_append(&ready_queue, &_thread->tnode);

    arch_irq_unlock(key);
    return 0;
}

int rt_schedule_remove(p_obj_t thread)
{

    return -1;
}
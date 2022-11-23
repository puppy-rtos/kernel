/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

p_list_t ready_queue = P_LIST_STATIC_INIT(&ready_queue);
struct _thread_obj *_g_curr_thread;
struct _thread_obj *_g_next_thread;
static int sched_lock = 0;
int p_sched(void)
{
    struct _thread_obj *_thread;
    p_base_t key = arch_irq_lock();

    if (sched_lock == 0)
    {
        if (!_g_next_thread)
        {
            _thread = p_list_entry(ready_queue.head, struct _thread_obj, tnode);
            if (_thread == _g_curr_thread)
            {
                arch_irq_unlock(key);
                return 0;
            }
            _g_next_thread = _thread;
            p_list_remove(ready_queue.head);
        }

        arch_swap();
    }

    arch_irq_unlock(key);
    return 0;
}

void p_sched_lock(void)
{
    sched_lock = 1;
}
void p_sched_unlock(void)
{
    sched_lock = 0;
    p_sched();
}

int p_sched_insert(p_obj_t thread)
{
    struct _thread_obj *_thread = thread;
    p_base_t key = arch_irq_lock();

    _thread->state = P_THREAD_STATE_READY;
    p_list_append(&ready_queue, &_thread->tnode);

    arch_irq_unlock(key);
    return 0;
}

int p_sched_remove(p_obj_t thread)
{
    struct _thread_obj *_thread = thread;
    p_base_t key = arch_irq_lock();

    p_list_remove(&_thread->tnode);

    arch_irq_unlock(key);
    return 0;
}

void p_sched_swap_out_cb(p_obj_t thread)
{
    
    _g_curr_thread = NULL;
}

void p_sched_swap_in_cb(p_obj_t thread)
{
    _g_next_thread->state = P_THREAD_STATE_RUN;
    _g_curr_thread = _g_next_thread;
    _g_next_thread = NULL;
}
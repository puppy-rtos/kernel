/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

#define KLOG_TAG  "sched"
#define KLOG_LVL   KLOG_WARNING
#include <puppy/klog.h>

static p_list_t ready_queue = P_LIST_STATIC_INIT(&ready_queue);
struct _thread_obj *_g_curr_thread;
struct _thread_obj *_g_next_thread;
static atomic_int _sched_lock = 1;
int p_sched(void)
{
    int ret = P_EOK;
    struct _thread_obj *_thread;
    p_base_t key = arch_irq_lock();

    if (_sched_lock == 0)
    {
        if (!_g_next_thread)
        {
            /* get prio higest thread */
            KLOG_ASSERT(p_list_is_empty(&ready_queue) == false);
            if (p_list_is_empty(&ready_queue))
            {
                arch_irq_unlock(key);
                return 0;
            }
            _thread = p_list_entry(ready_queue.head, struct _thread_obj, tnode);
            KLOG_ASSERT(p_obj_get_type(_thread) == P_OBJ_TYPE_THREAD);
            if (_g_curr_thread && _g_curr_thread->state == P_THREAD_STATE_RUN && _thread->prio >= _g_curr_thread->prio)
            {
                arch_irq_unlock(key);
                return 0;
            }
            _g_next_thread = _thread;
            p_list_remove(ready_queue.head);
            KLOG_D("next thread:%s", _g_next_thread->kobj.name);
            if (_g_curr_thread && _g_curr_thread->state == P_THREAD_STATE_RUN)
            {
                _g_curr_thread->state = P_THREAD_STATE_READY;
                p_sched_ready_insert(_g_curr_thread);
            }
        }

        arch_swap(key);
        ret = p_get_errno();
    }

    arch_irq_unlock(key);
    return -ret;
}

void p_sched_lock(void)
{
    atomic_fetch_add(&_sched_lock, 1);
}
void p_sched_unlock(void)
{
    atomic_fetch_sub(&_sched_lock, 1);
    if (!_sched_lock)
    {
        p_sched();
    }
}

int p_sched_ready_insert(p_obj_t thread)
{
    struct _thread_obj *old_thread = NULL, *temp_thread = NULL,*_thread = thread;
    p_base_t key = arch_irq_lock();

    p_node_t *node;
    KLOG_ASSERT(_thread != NULL);
    KLOG_ASSERT(p_obj_get_type(_thread) == P_OBJ_TYPE_THREAD);
    
    KLOG_D("p_sched_ready_insert:thread:%x,key:%x", _thread, key);
    
    p_list_for_each_node(&ready_queue, node)
    {
        temp_thread = p_list_entry(node, struct _thread_obj, tnode);
        KLOG_ASSERT(p_obj_get_type(temp_thread) == P_OBJ_TYPE_THREAD);
        if (temp_thread->prio > _thread->prio)
        {
            /* find out insert node */
            old_thread = temp_thread;
            break;
        }
    }
    

    _thread->state = P_THREAD_STATE_READY;
    if (old_thread)
    {
        p_list_insert(&old_thread->tnode, &_thread->tnode);
    }
    else
    {
        p_list_append(&ready_queue, &_thread->tnode);
    }
    
    KLOG_D("p_sched_ready_insert done:ready_queue.head:%x", ready_queue.head);

    arch_irq_unlock(key);
    return 0;
}

int p_sched_remove(p_obj_t thread)
{
    struct _thread_obj *_thread = thread;
    p_base_t key = arch_irq_lock();
    
    KLOG_ASSERT(p_obj_get_type(_thread) == P_OBJ_TYPE_THREAD);

    KLOG_D("p_sched_remove:tnode:%x",&_thread->tnode);
    p_list_remove(&_thread->tnode);

    arch_irq_unlock(key);
    return 0;
}

void p_sched_swap_out_cb(p_obj_t thread)
{
    P_UNUSED(thread);
    KLOG_ASSERT(p_obj_get_type(_g_curr_thread) == P_OBJ_TYPE_THREAD);
    _g_curr_thread = NULL;
}

void p_sched_swap_in_cb(p_obj_t thread)
{
    P_UNUSED(thread);
    KLOG_ASSERT(p_obj_get_type(_g_next_thread) == P_OBJ_TYPE_THREAD);
    _g_next_thread->state = P_THREAD_STATE_RUN;
    _g_curr_thread = _g_next_thread;
    _g_next_thread = NULL;
}
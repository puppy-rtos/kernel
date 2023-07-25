/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

#define KLOG_TAG  "sched"
#define KLOG_LVL   KLOG_WARNING
#include <puppy/klog.h>

struct _thread_obj *p_sched_ready_highest(void);

int p_sched(void)
{
    int ret = P_EOK;
    struct _thread_obj *_h_thread;
    p_base_t key = arch_irq_lock();
    struct p_cpu *_cpu = p_cpu_self();

    if (_cpu->sched_lock == 0)
    {
        if (!_cpu->next_thread)
        {
            /* get prio higest thread */
            _h_thread = p_sched_ready_highest();
            if (!_h_thread)
            {
                arch_irq_unlock(key);
                return 0;
            }
            if (_cpu->curr_thread && _cpu->curr_thread->state == P_THREAD_STATE_RUN && _h_thread->prio >= _cpu->curr_thread->prio)
            {
                arch_irq_unlock(key);
                return 0;
            }
            _cpu->next_thread = _h_thread;
            p_sched_remove(_h_thread);
            KLOG_D("next thread:%s", _cpu->next_thread->kobj.name);
            if (_cpu->curr_thread && _cpu->curr_thread->state == P_THREAD_STATE_RUN)
            {
                _cpu->curr_thread->state = P_THREAD_STATE_READY;
                p_sched_ready_insert(_cpu->curr_thread);
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
    atomic_fetch_add(&p_cpu_self()->sched_lock, 1);
}
void p_sched_unlock(void)
{
    atomic_fetch_sub(&p_cpu_self()->sched_lock, 1);
    if (!p_cpu_self()->sched_lock)
    {
        p_sched();
    }
}

int p_sched_ready_insert(p_obj_t thread)
{
    struct _thread_obj *old_thread = NULL, *temp_thread = NULL,*_thread = thread;
    p_base_t key = arch_irq_lock();

    p_node_t *node;
    p_list_t *_ready_queue = &p_cpu_self()->ready_queue;
    KLOG_ASSERT(_thread != NULL);
    KLOG_ASSERT(p_obj_get_type(_thread) == P_OBJ_TYPE_THREAD);
    
    KLOG_D("p_sched_ready_insert:thread:%x,key:%x,bindcpu:%d", _thread, key, _thread->bindcpu);
    if (_thread->bindcpu != CPU_NA) //todo support cpu_mask
    {
        _ready_queue = &p_cpu_index(_thread->bindcpu)->ready_queue;
    }
    
    p_list_for_each_node(_ready_queue, node)
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
        p_list_append(_ready_queue, &_thread->tnode);
    }
    
    KLOG_D("p_sched_ready_insert done:_ready_queue->head:%x", _ready_queue->head);

    arch_irq_unlock(key);
    return 0;
}

struct _thread_obj *p_sched_ready_highest(void)
{
    struct _thread_obj *highest_thread = NULL;
    p_base_t key = arch_irq_lock();

    p_list_t *_ready_queue = &p_cpu_self()->ready_queue;

    highest_thread = p_list_entry(_ready_queue->head, struct _thread_obj, tnode);
    KLOG_ASSERT(highest_thread != NULL);

    arch_irq_unlock(key);
    return highest_thread;
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
    KLOG_ASSERT(p_obj_get_type(p_cpu_self()->curr_thread) == P_OBJ_TYPE_THREAD);
    p_cpu_self()->curr_thread = NULL;
}

void p_sched_swap_in_cb(p_obj_t thread)
{
    struct p_cpu *cpu = p_cpu_self();
    P_UNUSED(thread);
    KLOG_ASSERT(p_obj_get_type(p_cpu_self()->next_thread) == P_OBJ_TYPE_THREAD);
    cpu->next_thread->state = P_THREAD_STATE_RUN;
    cpu->curr_thread = cpu->next_thread;
    cpu->next_thread = NULL;
    cpu->curr_thread->oncpu = p_cpu_self_id();
}
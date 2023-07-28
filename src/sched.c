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
            p_sched_ready_remove(_h_thread);
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

int sched_getcpu(void)
{
    return p_cpu_self_id();
}

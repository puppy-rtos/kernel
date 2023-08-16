/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <puppy/kobj.h>

#define KLOG_TAG  "sched_fool"
#define KLOG_LVL   KLOG_WARNING
#include <puppy/klog.h>

int p_sched_ready_insert(p_obj_t thread)
{
    static uint8_t cpuid_last = 0;
    struct _thread_obj *old_thread = NULL, *temp_thread = NULL,*_thread = thread;
    p_base_t key = arch_irq_lock();
    arch_spin_lock(&cpu);

    p_node_t *node;
    p_list_t *_ready_queue;
    KLOG_ASSERT(_thread != NULL);
    KLOG_ASSERT(p_obj_get_type(_thread) == P_OBJ_TYPE_THREAD);

    KLOG_D("p_sched_ready_insert:thread:%x,key:%x,bindcpu:%d", _thread, key, _thread->bindcpu);
    
#if P_CPU_NR > 1  // todo
    uint8_t need_send = CPU_NA;
    if (_thread->oncpu != CPU_NA)
    {
        cpuid_last = _thread->oncpu;
    }
    else if (_thread->bindcpu != CPU_NA) //todo support cpu_mask
    {
        cpuid_last = _thread->bindcpu;
    }
#endif
    _ready_queue = &p_cpu_index(cpuid_last)->ready_queue;

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

#if P_CPU_NR > 1  // todo
    if(cpuid_last != p_cpu_self_id())
    {
        KLOG_D("need send ipi");
        need_send = cpuid_last;
    }
    cpuid_last = (cpuid_last + 1) % P_CPU_NR;
#endif

    arch_spin_unlock(&cpu);
    arch_irq_unlock(key);
    
#if P_CPU_NR > 1  // todo
    if(need_send != CPU_NA)
    {
        void arch_ipi_send(uint8_t cpuid);
        arch_ipi_send(need_send);
    }
#endif

    return 0;
}

struct _thread_obj *p_sched_ready_highest(void)
{
    struct _thread_obj *highest_thread = NULL;
    p_base_t key = arch_irq_lock();
    arch_spin_lock(&cpu);

    p_list_t *_ready_queue = &p_cpu_self()->ready_queue;

    if (!p_list_is_empty(_ready_queue))
    {
        highest_thread = p_list_entry(_ready_queue->head, struct _thread_obj, tnode);
        KLOG_ASSERT(highest_thread != NULL);
        KLOG_ASSERT(p_obj_get_type(highest_thread) == P_OBJ_TYPE_THREAD);
    }

    arch_spin_unlock(&cpu);
    arch_irq_unlock(key);
    return highest_thread;
}

int p_sched_ready_remove(p_obj_t thread)
{
    struct _thread_obj *_thread = thread;
    p_base_t key = arch_irq_lock();
    
    KLOG_ASSERT(p_obj_get_type(_thread) == P_OBJ_TYPE_THREAD);

    KLOG_D("p_sched_ready_remove:tnode:%x",&_thread->tnode);
    p_list_remove(&_thread->tnode);

    arch_irq_unlock(key);
    return 0;
}

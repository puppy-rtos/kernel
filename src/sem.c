/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <puppy.h>

#define KLOG_TAG  "sem"
#define KLOG_LVL   KLOG_WARNING
#include <puppy/klog.h>

void p_sem_init(p_obj_t obj, const char *name,
                     uint32_t    init_value,
                     uint32_t    max_value)
{
    struct _sem_obj *sem = obj;
    
    p_obj_init(obj, name, P_OBJ_TYPE_IPC, P_OBJ_TYPE_IPC_SEM);
    
    sem->kobj.name = name;
    sem->value = init_value;
    sem->max_value = max_value;
    p_list_init(&sem->blocking_list);
}

p_obj_t p_sem_create(const char *name,
                     uint32_t    init_value,
                     uint32_t    max_value)
{
    return NULL;
}

void _block_thread(p_list_t *list, struct _thread_obj *thread)
{
    struct _thread_obj *temp_thread;
    if (p_list_is_empty(list))
    {
        p_list_append(list, &thread->tnode);
    }
    else
    {
        p_node_t *pos = list->head;
        while(pos != list) 
        {
            temp_thread = p_list_entry(pos, struct _thread_obj, tnode);
            KLOG_ASSERT(p_obj_get_type(temp_thread) == P_OBJ_TYPE_THREAD);
            if (temp_thread->prio > thread->prio)
            {
                break;
            }
			pos = pos->next;
		}
        if (pos != list)
        {
            p_list_insert(pos, &thread->tnode);
        }
        else
        {
            p_list_append(list, &thread->tnode);
        }
    }
    KLOG_D("_block_thread:%s", thread->kobj.name);
    p_thread_block(thread);
}

void _wakeup_block_thread(p_list_t *list)
{
    struct _thread_obj *_thread;
    _thread = p_list_entry(list->head,
                            struct _thread_obj, tnode);
    p_list_remove(&_thread->tnode);
    KLOG_D("_wakeup_block_thread:%s", _thread->kobj.name);
    p_sched_ready_insert(_thread);
}

int p_sem_post(p_obj_t obj)
{
    struct _sem_obj *sem = obj;
    p_base_t key = arch_irq_lock();
    KLOG_ASSERT(p_obj_get_type(sem) == P_OBJ_TYPE_IPC);
    KLOG_ASSERT(p_obj_get_extype(sem) == P_OBJ_TYPE_IPC_SEM);

    if (sem->value < sem->max_value)
    {
        sem->value ++;
    }
    if (p_list_is_empty(&sem->blocking_list))
    {
        goto _exit;
    }
    _wakeup_block_thread(&sem->blocking_list);
    p_sched();
_exit:
    arch_irq_unlock(key);
    return -P_ENOSYS;
}

int p_sem_timewait(p_obj_t obj, p_tick_t tick)
{
    return -P_ENOSYS;
}

int p_sem_wait(p_obj_t obj)
{
    struct _sem_obj *sem = obj;
    int ret = P_EOK;
    p_base_t key = arch_irq_lock();
    if (sem->value > 0)
    {
        sem->value --;
        goto _exit;
    }
    _block_thread(&sem->blocking_list, p_thread_self());
    ret = p_sched();
    if (ret == P_EOK)
    {
        sem->value --;
    }
_exit:
    arch_irq_unlock(key);
    return ret;
}

int p_sem_control(p_obj_t obj, int cmd, void *argv)
{
    return -P_ENOSYS;
}
int p_sem_delete(p_obj_t obj)
{
    return -P_ENOSYS;
}


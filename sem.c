/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <puppy.h>

struct _sem_obj
{
    struct p_obj kobj;
    uint16_t    value;
    uint16_t    max_value;
    p_list_t    blocking_list;
};

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
int p_sem_post(p_obj_t obj)
{
    struct _sem_obj *sem = obj;
    p_base_t key = arch_irq_lock();
    P_ASSERT(p_obj_get_type(sem) == P_OBJ_TYPE_IPC);
    P_ASSERT(p_obj_get_extype(sem) == P_OBJ_TYPE_IPC_SEM);

    if (sem->value < sem->max_value)
    {
        sem->value ++;
    }
    if (p_list_is_empty(&sem->blocking_list))
    {
        goto _exit;
    }
    /* todo */
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
    return -P_ENOSYS;
}
int p_sem_control(p_obj_t obj, int cmd, void *argv)
{
    return -P_ENOSYS;
}
int p_sem_delete(p_obj_t obj)
{
    return -P_ENOSYS;
}


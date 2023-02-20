/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <string.h>
#include <puppy/syscall.h>

#define KLOG_TAG  "syscall"
#define KLOG_LVL   KLOG_INFO
#include <puppy/klog.h>

void p_tick_init(int tick_persec)
{
    arch_syscall(_NRSYS_tick_init, tick_persec);
}

void p_tick_inc(void)
{
    arch_syscall(_NRSYS_tick_inc);
}

p_tick_t p_tick_get(void)
{
    return arch_syscall(_NRSYS_tick_get);
}

int p_tick_persec(void)
{
    return arch_syscall(_NRSYS_tick_persec);
}

void p_thread_init(p_obj_t obj, const char *name,
                                void (*entry)(void *param),
                                void    *param,
                                void    *stack_addr,
                                uint32_t stack_size,
                                uint8_t  prio)
{
    void *arg[] = {obj, name, entry, param, stack_addr, stack_size, prio};
    arch_syscall(_NRSYS_thread_init, arg);
}

int p_thread_start(p_obj_t obj)
{
    return arch_syscall(_NRSYS_thread_start, obj);
}

int p_thread_yield(void)
{
    return arch_syscall(_NRSYS_thread_yield);
}

int p_thread_block(p_obj_t obj)
{
    return arch_syscall(_NRSYS_thread_block, obj);
}

int p_thread_wakeup(p_obj_t obj)
{
    return arch_syscall(_NRSYS_thread_wakeup, obj);
}

int p_thread_sleep(p_tick_t tick)
{
    return arch_syscall(_NRSYS_thread_sleep, tick);
}

p_obj_t p_thread_self(void)
{
    return arch_syscall(_NRSYS_thread_self);
}

char *p_thread_self_name(void)
{
    return arch_syscall(_NRSYS_thread_self_name);
}

int p_thread_abort(p_obj_t obj)
{
    return arch_syscall(_NRSYS_thread_abort, obj);
}

void p_sem_init(p_obj_t obj, const char *name,
                     uint32_t    init_value,
                     uint32_t    max_value)
{
    arch_syscall(_NRSYS_sem_init, obj, name, init_value, max_value);
}

p_obj_t p_sem_create(const char *name,
                     uint32_t    init_value,
                     uint32_t    max_value)
{
    return arch_syscall(_NRSYS_sem_create, name, init_value, max_value);
}

int p_sem_post(p_obj_t obj)
{
    return arch_syscall(_NRSYS_sem_post, obj);
}

int p_sem_timewait(p_obj_t obj, p_tick_t tick)
{
    return arch_syscall(_NRSYS_sem_timewait, obj, tick);
}

int p_sem_wait(p_obj_t obj)
{
    return arch_syscall(_NRSYS_sem_wait, obj);
}

int p_sem_control(p_obj_t obj, int cmd, void *argv)
{
    return arch_syscall(_NRSYS_sem_control, obj, cmd, argv);
}

int p_sem_delete(p_obj_t obj)
{
    return arch_syscall(_NRSYS_sem_delete, obj);
}

/* NRSYS\((.*)\) * p_$1(...)\n{\n    return arch_syscall(_NRSYS_$1, ...);\n}\n */

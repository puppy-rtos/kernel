/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <string.h>

#define P_THREAD_SLICE_DEFAULT 10

static p_list_t thread_timeout_list = P_LIST_STATIC_INIT(&thread_timeout_list);
static void timeout_insert(struct timeout *timeout);
static int timeout_remove(struct timeout *timeout);
extern struct _thread_obj *_g_curr_thread;

void p_thread_entry(void (*entry)(void *parameter), void *param)
{
    if (entry)
    {
        entry(param);
    }
    p_thread_abort(p_thread_self());
    while (1);
}

void p_thread_init(p_obj_t obj, const char *name,
                                void (*entry)(void *param),
                                void    *param,
                                void    *stack_addr,
                                uint32_t stack_size,
                                uint8_t  prio)
{
    struct _thread_obj *thread = obj;
    

    p_obj_init(&thread->kobj, name, P_OBJ_TYPE_THREAD | P_OBJ_TYPE_STATIC, NULL);
    
    thread->entry = entry;
    thread->param = param;
    thread->stack_addr = stack_addr;
    thread->stack_size = stack_size;
    thread->prio = prio;
    thread->slice_ticks = P_THREAD_SLICE_DEFAULT;
    thread->state = P_THREAD_STATE_INIT;
    
    memset(stack_addr,0x23, stack_size);

    arch_new_thread(thread, stack_addr, stack_size);
}

p_obj_t p_thread_self(void)
{
    P_ASSERT(_g_curr_thread != NULL);
    P_ASSERT(p_obj_get_type(_g_curr_thread) == P_OBJ_TYPE_THREAD);
    return _g_curr_thread;
}

int p_thread_abort(p_obj_t obj)
{
    struct _thread_obj *_thread = _g_curr_thread;
    p_base_t key = arch_irq_lock();
    P_ASSERT(p_obj_get_type(_thread) == P_OBJ_TYPE_THREAD);

    _thread->state = P_THREAD_STATE_DEAD;
    timeout_remove(&_thread->timeout);
    p_sched();
    
    arch_irq_unlock(key);
    return 0;
}

int p_thread_yield(void)
{
    struct _thread_obj *_thread = _g_curr_thread;
    p_base_t key = arch_irq_lock();
    
    P_ASSERT(p_obj_get_type(_thread) == P_OBJ_TYPE_THREAD);
    P_ASSERT(_thread->state == P_THREAD_STATE_RUN);

    p_sched_insert(_thread);
    p_sched();
    
    arch_irq_unlock(key);
    return 0;
}

void sleep_timeout_func(p_obj_t obj, void *param)
{
    struct _thread_obj *_thread = obj;
    _thread->error = -P_ETIMEOUT;
    p_thread_resume(_thread);
    p_sched();
}

int p_thread_sleep(p_tick_t tick)
{
    struct _thread_obj *_thread = _g_curr_thread;
    int err = P_EOK;
    p_base_t key = arch_irq_lock();

    P_ASSERT(p_obj_get_type(_thread) == P_OBJ_TYPE_THREAD);
    P_ASSERT(_thread->state == P_THREAD_STATE_RUN);

    _thread->timeout.tick = p_tick_get() + tick;
    _thread->timeout.func = sleep_timeout_func;
    _thread->timeout.param = NULL;
    timeout_insert(&_thread->timeout);
    p_thread_suspend(_thread);
    p_sched();

_exit:
    arch_irq_unlock(key);
    return err;
}

int p_thread_suspend(p_obj_t obj)
{
    struct _thread_obj *_thread = obj;
    int err = P_EOK;
    p_base_t key = arch_irq_lock();
    P_ASSERT(p_obj_get_type(_thread) == P_OBJ_TYPE_THREAD);
    P_ASSERT(_thread->state == P_THREAD_STATE_RUN);

    if (_thread->state != P_THREAD_STATE_RUN)
    {
        err = -P_EINVAL;
        goto _exit;
    }
    _thread->state = P_THREAD_STATE_SLEEP;

_exit:
    arch_irq_unlock(key);
    return err;
}

int p_thread_resume(p_obj_t obj)
{
    struct _thread_obj *_thread = obj;
    int err = P_EOK;
    p_base_t key = arch_irq_lock();
    P_ASSERT(p_obj_get_type(_thread) == P_OBJ_TYPE_THREAD);
    P_ASSERT(_thread->state == P_THREAD_STATE_SLEEP);

    if (_thread->state != P_THREAD_STATE_SLEEP)
    {
        err = -P_EINVAL;
        goto _exit;
    }
    p_sched_insert(_thread);

_exit:
    arch_irq_unlock(key);
    return err;
}
int p_thread_block(p_obj_t obj)
{
    
    return -P_ENOSYS;
}
int p_thread_start(p_obj_t obj)
{
    P_ASSERT(p_obj_get_type(obj) == P_OBJ_TYPE_THREAD);
    // struct _thread_obj *thread = obj;
    
    p_sched_insert(obj);
    
    p_sched();
    return 0;
}


/******** thread timeout ********/
static struct timeout *_next_timeout(void)
{
    struct timeout *timeout;
    p_base_t key = arch_irq_lock();

    if (!p_list_is_empty(&thread_timeout_list))
    {
        timeout = p_list_entry(thread_timeout_list.head,
                             struct timeout, node);
        arch_irq_unlock(key);
        return timeout;
    }

    arch_irq_unlock(key);

    return NULL;
}

static void timeout_insert(struct timeout *timeout)
{
    struct timeout *_timeout = NULL, *temp_timeout;
    p_base_t key = arch_irq_lock();
    p_node_t *node;
    
    p_list_for_each_node(&thread_timeout_list, node)
    {
        temp_timeout = p_list_entry(node, struct timeout, node);
        if (temp_timeout->tick > timeout->tick)
        {
            /* find out insert node */
            _timeout = temp_timeout;
            break;
        }
    }

    if (_timeout)
    {
        p_list_insert(&_timeout->node, &timeout->node);
    }
    else
    {
        p_list_append(&thread_timeout_list, &timeout->node);
    }

    arch_irq_unlock(key);
}
static int timeout_remove(struct timeout *timeout)
{
    p_base_t key = arch_irq_lock();

    p_list_remove(&timeout->node);

    arch_irq_unlock(key);
    return 0;
}
void thread_timeout_cb(p_base_t tick)
{
    struct timeout *timeout;
    struct _thread_obj *_thread;
_next:
    timeout = _next_timeout();
    if(timeout && timeout->tick <= tick)
    {
        _thread = p_list_entry(timeout,
                            struct _thread_obj, timeout);
        
        P_ASSERT(p_obj_get_type(_thread) == P_OBJ_TYPE_THREAD);
        /* timeout ok */
        p_list_remove(&timeout->node);
        /* todo */
        if (timeout->func)
        {
            timeout->func(_thread, timeout->param);
        }
        goto _next;
    }
}
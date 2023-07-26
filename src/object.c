/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <string.h>

#define KLOG_TAG  "obj"
#define KLOG_LVL   KLOG_WARNING
#include <puppy/klog.h>

static p_list_t _g_obj_list = P_LIST_STATIC_INIT(&_g_obj_list);

void p_obj_init(p_obj_t obj, const char *name, uint8_t type, uint8_t ex_type)
{
    struct p_obj *object = obj;
    p_base_t key;

    KLOG_D("p_obj_init --> [%s], type:%x, ex_type:%x...", name, type, ex_type);
    object->name = name;
    type = type & (P_OBJ_TYPE_MASK | P_OBJ_TYPE_STATIC);
    object->type = (uint16_t)type | ((uint16_t)ex_type << 8);
    key = arch_irq_lock();
    p_list_append(&_g_obj_list, &object->node);
    arch_irq_unlock(key);
    KLOG_D("done");
}

uint8_t p_obj_get_type(p_obj_t obj)
{
    struct p_obj *object = obj;
    
    return object->type & P_OBJ_TYPE_MASK;
}
uint8_t p_obj_get_extype(p_obj_t obj)
{
    struct p_obj *object = obj;
    
    return object->type >> 8;
}
void p_obj_deinit(p_obj_t obj)
{
    struct p_obj *object = obj;
    p_base_t key;
    KLOG_D("p_obj_deinit --> [%s]...", object->name);
    key = arch_irq_lock();
    if (object->type & P_OBJ_TYPE_STATIC)
    {
        p_list_remove(&object->node);
    }
    arch_irq_unlock(key);
    KLOG_D("done");
}

p_obj_t p_obj_find(const char *name)
{
    p_node_t *node;
    struct p_obj *object;
    
    p_list_for_each_node(&_g_obj_list, node)
    {
        object = p_list_entry(node, struct p_obj, node);
        if (strncmp(object->name, name, INT32_MAX - 1) == 0)
        {
            return object;
        }
    }
    return NULL;
}

void list_thread(void)
{
    p_node_t *node;
    struct p_obj *object;
    int maxlen;

    maxlen = 8;

    printk("thread   cpu  bind  pri  state   stack size max used left tick  error\n");
    printk("-------  ---  ----  ---  ------- ----------  ------  ---------- ------\n");

    p_list_for_each_node(&_g_obj_list, node)
    {
        object = p_list_entry(node, struct p_obj, node);
        if (p_obj_get_type(object) != P_OBJ_TYPE_THREAD)
        {
            continue;
        }
        {
            uint8_t stat;
            uint8_t *ptr;
            struct _thread_obj *thread = (struct _thread_obj *)object;

            if (thread->oncpu != CPU_NA)
            {
                printk("%-*.*s %3d  %3d   %3d ", maxlen, maxlen, thread->kobj.name, thread->oncpu, thread->bindcpu, thread->prio);
            }
            else
            {
                printk("%-*.*s %s  %3d   %3d ", maxlen, maxlen, thread->kobj.name, "N/A", thread->bindcpu, thread->prio);
            }

            stat = thread->state;
            if (stat == P_THREAD_STATE_READY)        printk(" ready  ");
            else if (stat == P_THREAD_STATE_BLOCK)   printk(" blocked");
            else if (stat == P_THREAD_STATE_INIT)    printk(" init   ");
            else if (stat == P_THREAD_STATE_DEAD)    printk(" dead   ");
            else if (stat == P_THREAD_STATE_RUN)     printk(" running");
            else if (stat == P_THREAD_STATE_SLEEP)   printk(" sleep  ");

            ptr = (uint8_t *)thread->stack_addr;
            while (*ptr == '#') ptr ++;
            printk(" 0x%08x    %02d%%   0x%08x %s\n",
                        thread->stack_size,
                        (thread->stack_size - ((p_ubase_t) ptr - (p_ubase_t) thread->stack_addr)) * 100
                        / thread->stack_size,
                        thread->slice_ticks,
                        p_errno_str(thread->errno));
        }
    }
}

/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <string.h>

static p_list_t _g_obj_list = P_LIST_STATIC_INIT(&_g_obj_list);

void p_obj_init(p_obj_t obj, const char *name, uint8_t type)
{
    struct p_obj *object = obj;
    p_base_t key;
    object->name = name;
    object->type = type & (P_OBJ_TYPE_MASK | P_OBJ_TYPE_STATIC);
    key = arch_irq_lock();
    p_list_append(&_g_obj_list, &object->node);
    arch_irq_unlock(key);
}

uint8_t p_obj_get_type(p_obj_t obj)
{
    struct p_obj *object = obj;
    
    return object->type & P_OBJ_TYPE_MASK;
}
void p_obj_deinit(p_obj_t obj)
{
    struct p_obj *object = obj;
    p_base_t key;
    key = arch_irq_lock();
    if (object->type & P_OBJ_TYPE_STATIC)
    {
        p_list_remove(&object->node);
    }
    arch_irq_unlock(key);
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

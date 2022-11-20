/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

static p_list_t _g_obj_list = P_LIST_STATIC_INIT(&_g_obj_list);

void p_obj_init(p_obj_t obj, const char *name, uint8_t type)
{
    struct p_obj *object = obj;
    object->name = name;
    object->type = type & (P_OBJ_TYPE_MASK | P_OBJ_TYPE_STATIC);

    p_list_append(&_g_obj_list, &object->node);
}

/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
#include <puppy.h>

static void kobj_tc(void)
{
    p_obj_t     p_obj_t_value;     /**<  8bit integer type */
    int         err;

    err = P_EOK     ;
    err = P_ERROR   ;
    err = P_EINVAL  ;
    err = P_EISR    ;
    err = P_ETIMEOUT;
    err = P_ENOMEM  ;
    err = P_EBUSY   ;
    err = P_ENOSYS  ;
    err = P_ESYSCALL;

    P_UNUSED(p_obj_t_value);
    P_UNUSED(err);
}

static struct _klist_tc_t
{
    int index;
    p_node_t node;
}_tc_list[3];

static void klist_tc(void)
{
    p_list_t list = P_LIST_STATIC_INIT(&list);
    int i;
    p_node_t *node, *node_s;
    struct _klist_tc_t *list_tc_ptr;

    if (p_list_is_empty(&list) != true)
    {
        P_TC_FAIL();
    }

    for (i = 0; i < 3; i ++)
    {
        _tc_list[i].index = i;
        p_list_append(&list, &_tc_list[i].node);
    }

    i = 0;
    p_list_for_each_node(&list, node)
    {
        list_tc_ptr = p_container_of(node, struct _klist_tc_t, node);
        if (list_tc_ptr->index != i)
        {
            P_TC_FAIL();
            return;
        }
        i ++;
    }

    p_list_for_each_node_safe(&list, node, node_s)
    {
        p_list_remove(node);
    }

    if (p_list_is_empty(&list) == false)
    {
        P_TC_FAIL();
    }
    P_TC_PASS();
}

static void kobj_api_tc(void)
{
    P_TC_PASS();
}

void kobj_tc_main(void)
{
    klist_tc();
}

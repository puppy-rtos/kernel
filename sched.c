/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

p_list_t ready_queue = P_LIST_STATIC_INIT(&ready_queue);

int p_schedule(void)
{
    return -1;
}

int p_schedule_insert(p_obj_t thread)
{
    return -1;
}
int rt_schedule_remove(p_obj_t thread)
{

    return -1;
}
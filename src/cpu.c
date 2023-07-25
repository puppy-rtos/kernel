/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

#define KLOG_TAG  "cpu"
#define KLOG_LVL   KLOG_WARNING
#include <puppy/klog.h>

static struct p_cpu _g_cpu[CPU_NR];

p_weak uint8_t p_cpu_self_id(void)
{
    return 0;
}

struct p_cpu *p_cpu_self(void)
{
    return &_g_cpu[p_cpu_self_id()];
}

struct p_cpu *p_cpu_index(uint8_t cpuid)
{
    if (cpuid >= CPU_NR)
        return NULL;
    return &_g_cpu[cpuid];
}

void p_cpu_init(void)
{
    for (int i = 0; i < CPU_NR; i++)
    {
        p_list_init(&_g_cpu[i].ready_queue);
        _g_cpu[i].sched_lock = 1;
    }
}

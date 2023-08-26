/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

static atomic_int _g_tick;
static int _tick_persec;
static int _latency_perus;

static uint32_t latency_max = 0;
static uint32_t latency_avg = 0;
static uint32_t latency_sum = 0;

void p_tick_init(int tick_persec, int latency_perus)
{
    _tick_persec = tick_persec;
    _latency_perus = latency_perus;
}

void p_tick_inc(int tick, int latency)
{
    if ((_g_tick % 1024) == 0)
    {
        latency_avg = (latency_avg + (latency_sum >> 10)) >> 1;
        latency_sum = latency;
    }
    else
    {
        latency_sum += latency;
    }
    if (latency > latency_max)
    {
        latency_max = latency;
    }
    atomic_fetch_add(&_g_tick, tick);
    thread_timeout_cb(_g_tick);
}

p_tick_t p_tick_get(void)
{
    return _g_tick;
}

int p_tick_persec(void)
{
    return _tick_persec;
}

#if defined(ENABLE_NR_SHELL)
#include <nr_micro_shell.h>
void shell_tickdump_cmd(char argc, char *argv)
{
    uint32_t avg = latency_sum / ((_g_tick % 1024) + 1);
    if (latency_avg != 0)
    {
        avg = (avg + latency_avg) >> 1;
    }
    printk("Current tick:%d\n", _g_tick);
    avg = avg * 100 / _latency_perus;
    printk("Average Latency:%d.%d us\n", avg / 100, avg % 100);
    avg = latency_max * 100 / _latency_perus;
    printk("Max     Latency:%d.%d us\n", avg / 100, avg % 100);
}
NR_SHELL_CMD_EXPORT(tick_dump, shell_tickdump_cmd);
#endif

/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

static p_atomic_t _g_tick;
static int _tick_persec;

void p_tick_init(int tick_persec)
{
    _tick_persec = tick_persec;
}

void p_tick_inc(void)
{
    arch_atomic_add(&_g_tick, 1);
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
/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

static int _g_errno;

int p_get_errno(void)
{
    struct _thread_obj *_thread = p_thread_self();
    if (_thread)
        return _thread->errno;
    else
        return _g_errno;
}

void p_set_errno(int errno)
{
    struct _thread_obj *_thread = p_thread_self();
    if (_thread)
        _thread->errno = errno;
    else
        _g_errno = errno;
}

const char *p_errno_str(int errno)
{
    return "unknown";
}
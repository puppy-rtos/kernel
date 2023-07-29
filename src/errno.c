/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <puppy/kobj.h>

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
    if (errno == P_EOK)            return" OK";
    else if (errno == P_EINVAL)    return" INVAL";
    else if (errno == P_EISR)      return" ISR";
    else if (errno == P_ETIMEOUT)  return" TIMEOUT";
    else if (errno == P_EBUSY)     return" BUSY";
    else if (errno == P_ENOSYS)    return" NOSYS";
    else if (errno == P_ESYSCALL)  return" SYSCALL";
    else return "unknown";
}
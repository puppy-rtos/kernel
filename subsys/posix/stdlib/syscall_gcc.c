/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <puppy.h>
#include <reent.h>
#include <unistd.h>
#include <sys/errno.h>

#define KLOG_TAG  "syscall.gcc"
#define KLOG_LVL   KLOG_WARNING
#include <puppy/klog.h>

void *_malloc_r(struct _reent *ptr, size_t size)
{
    void* result;

    result = (void*)p_malloc(size);
    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

    return result;
}

void *_realloc_r(struct _reent *ptr, void *old, size_t newlen)
{
    void* result;

    result = (void*)p_realloc(old, newlen);
    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

    return result;
}

void *_calloc_r(struct _reent *ptr, size_t size, size_t len)
{
    void* result;

    result = (void*)p_calloc(size, len);
    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

    return result;
}

void _free_r(struct _reent *ptr, void *addr)
{
    p_free(addr);
}

void __libc_init_array(void)
{
    /* we not use __libc init_aray to initialize C++ objects */
    /* __libc_init_array is ARM code, not Thumb; it will cause a hardfault. */
}

/* Reentrant versions of system calls.  */
#ifndef _REENT_ONLY
int *__errno(void)
{
  return p_get_errno();
}
#endif

/* for exit() and abort() */
__attribute__ ((noreturn)) void _exit (int status)
{
    while(1);
}

/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <puppy.h>
#include <stddef.h>

#define KLOG_TAG  "syscall.armclang"
#define KLOG_LVL   KLOG_WARNING
#include <puppy/klog.h>

#ifdef __CC_ARM
    /* avoid the heap and heap-using library functions supplied by arm */
    #pragma impop(__use_no_heap)
#endif /* __CC_ARM */

void *malloc(size_t n)
{
    return p_malloc(n);
}

void *realloc(void *rmem, size_t newsize)
{
    return p_realloc(rmem, newsize);
}

void *calloc(size_t nelem, size_t elsize)
{
    return p_calloc(nelem, elsize);
}

void free(void *rmem)
{
    p_free(rmem);
}

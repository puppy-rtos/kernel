/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PUPPY_INC_TOOLCHAN_H__
#define PUPPY_INC_TOOLCHAN_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdatomic.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__ARMCC_VERSION)           /* ARM Compiler */
typedef unsigned long ssize_t;
#endif

#define p_weak                   __attribute__((weak))
#define p_section(x)             __attribute__((section(x)))
#define p_used                   __attribute__((used))
#define p_align(n)               __attribute__((aligned(n)))

typedef union {
    long long       thelonglong;
    long double     thelongdouble;
    uintmax_t       theuintmax_t;
    size_t          thesize_t;
    uintptr_t       theuintptr_t;
    void            *thepvoid;
    void            (*thepfunc)(void);
} p_max_align_t;

#ifdef __cplusplus
}
#endif

#endif

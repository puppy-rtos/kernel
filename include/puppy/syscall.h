/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PUPPY_INC_SYSCALL_H__
#define PUPPY_INC_SYSCALL_H__

int syscall(int number, ...);

#define NRSYS(x) _NRSYS_##x,
enum
{
    _NRSYS_LOG = 0,

    #include "syscall_no.h"

    _NRSYS_SYSCALL_NR
};

#endif

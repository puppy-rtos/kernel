
/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PUPPY_INC_UTIL_H__
#define PUPPY_INC_UTIL_H__

#if 1
#include <stdio.h>
#define printk(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#else
#define printk(fmt, ...)
#endif

#define P_TC_PASS()         printk("Test Passed!\r\n");
#define P_TC_FAIL()         printk("Test Failed!\r\n"); 
#define P_TC_LOG(...)   do {printk(__VA_ARGS__); printk("\r\n");} while (0);

#endif

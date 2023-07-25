
/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PUPPY_INC_DEF_CONFIG_H__
#define PUPPY_INC_DEF_CONFIG_H__

#ifndef P_ALIGN_SIZE
#define P_ALIGN_SIZE  4
#endif

#ifndef P_CONFIG_PTHREAD_STACK_MIN
#define P_CONFIG_PTHREAD_STACK_MIN     512
#endif

#ifndef P_CONFIG_PTHREAD_STACK_DEFAULT
#define P_CONFIG_PTHREAD_STACK_DEFAULT 1024
#endif

#ifndef P_IDLE_THREAD_STACK_SIZE
#define P_IDLE_THREAD_STACK_SIZE  1024
#endif

#ifndef P_MAIN_THREAD_STACK_SIZE
#define P_MAIN_THREAD_STACK_SIZE  1024
#endif

#ifndef P_PRINTK_BUF_SIZE
#define P_PRINTK_BUF_SIZE  512
#endif

#define P_SCHED_PRIO_DEFAULT 12
#define P_NAME_MAX 8

#define KLOG_ENABLE
#define KLOG_COLOR
#define KLOG_TIME
#define KLOG_CPU
#define KLOG_THREAD
#define KLOG_LOCK

#endif

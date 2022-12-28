/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PUPPY_INC_KERNEL_H__
#define PUPPY_INC_KERNEL_H__

#define p_tick_init         k_tick_init
#define p_tick_inc          k_tick_inc
#define p_tick_get          k_tick_get
#define p_tick_persec       k_tick_persec
#define p_thread_init       k_thread_init
#define p_thread_start      k_thread_start
#define p_thread_yield      k_thread_yield
#define p_thread_block      k_thread_block
#define p_thread_wakeup     k_thread_wakeup
#define p_thread_sleep      k_thread_sleep
#define p_thread_self       k_thread_self
#define p_thread_self_name  k_thread_self_name
#define p_thread_abort      k_thread_abort
#define p_sem_init          k_sem_init
#define p_sem_create        k_sem_create
#define p_sem_post          k_sem_post
#define p_sem_timewait      k_sem_timewait
#define p_sem_wait          k_sem_wait
#define p_sem_control       k_sem_control
#define p_sem_delete        k_sem_delete

/* NRSYS\((.*)\) #define p_$1\t\tk_$1 */

#endif
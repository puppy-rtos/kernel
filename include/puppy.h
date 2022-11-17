/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __PUPPY_H__
#define __PUPPY_H__

#include <puppy/toolchan.h>

#define P_UNUSED(x)                   ((void)x)

/* Puppy-RTOS object definitions */
typedef void    *p_obj_t;
typedef ssize_t  p_err_t;
typedef size_t   p_tick_t;

/* Puppy-RTOS error code definitions */
#define P_EOK           0 /* There is no error */
#define P_ERROR         1 /* A unknow error happens */
#define P_EINVAL        2 /* Invalid argument */
#define P_EISR          3 /* Can't use in ISR */
#define P_ETIMEOUT      4 /* Timed out */
#define P_ENOMEM        5 /* No memory */
#define P_EBUSY         6 /**< Busy */
#define P_ENOSYS        7 /* No system */
#define P_ESYSCALL      8 /* Interrupted system call */

/**
 * thread api
 */
p_err_t p_get_errno(void);
void p_set_errno(p_err_t errno);
const char *p_errno_str(p_err_t errno);

/**
 * object api
 */
p_obj_t p_obj_find(const char *name);
p_obj_t p_obj_ioctl(const char *name);

/**
 * tick api
 */
void p_tick_init(int tick_persec);
void p_tick_inc(void);
p_tick_t p_tick_get(void);
int p_tick_persec(void);

/**
 * thread api
 */
#define P_THREAD_STATUS_INIT        0x00
#define P_THREAD_STATUS_SLEEP       0x01
#define P_THREAD_STATUS_BLOCK       0x02
#define P_THREAD_STATUS_RUNABLE     0x03
#define P_THREAD_STATUS_RUN         0x04
#define P_THREAD_STATUS_DEAD        0x05

typedef struct p_thread_attr
{
    const char *name;
    size_t      stack_size;
    p_tick_t    tick;
    uint8_t     priority; 
    uint8_t     reserved[3];
} p_thread_attr_t;

p_obj_t p_thread_create(const char *name,
                        void (*entry)(void *parameter),
                        void    *parameter,
                        uint32_t stack_size,
                        uint8_t  priority,
                        p_tick_t tick);
p_err_t p_thread_start(p_obj_t obj);
p_err_t p_thread_yield(void);
p_err_t p_thread_sleep(p_tick_t tick);
p_err_t p_thread_control(p_obj_t obj, uint32_t cmd, void *argv);
p_err_t p_thread_getattr(p_obj_t obj, p_thread_attr_t *attr);
/* todo: p_err_t p_thread_delete(p_obj_t obj); */
p_obj_t p_thread_self(void);

/**
 * sem api
 */
p_obj_t p_sem_create(const char *name,
                     uint32_t    init_value,
                     uint32_t    max_value);
p_err_t p_sem_post(p_obj_t obj);
p_err_t p_sem_timewait(p_obj_t obj, p_tick_t tick);
p_err_t p_sem_wait(p_obj_t obj);
p_err_t p_sem_control(p_obj_t obj, uint32_t cmd, void *argv);
p_err_t p_sem_delete(p_obj_t obj);


#include <puppy/util.h>

#endif

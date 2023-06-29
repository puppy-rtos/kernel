/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
#include <puppy.h>

static struct _thread_obj t1;
__attribute__((aligned(8)))
static uint8_t t1_thread_stack[1024];

static struct _thread_obj t2;
__attribute__((aligned(8)))
static uint8_t t2_thread_stack[1024];

static struct _sem_obj sem;
int _g_cnt = 0;

static void t1_thread_entry(void *parm)
{
    while(1)
    {
        p_sem_wait(&sem);
        printk("t1:waited sem %d\r\n", ++_g_cnt);
        if (_g_cnt == 10)
        {
            P_TC_PASS();
            return;
        }
    }
}

static void t2_thread_entry(void *parm)
{
    int cnt = 0;
    while(cnt < 10)
    {
        p_sem_post(&sem);
        printk("main:post sem %d\r\n", ++cnt);
        p_thread_sleep(100);
    }
}

void test_sem_api(void)
{
    p_sem_init(&sem, "sem", 0, 1);
    p_thread_init(&t1, "t1", t1_thread_entry, NULL,
                  t1_thread_stack,
                  sizeof(t1_thread_stack),
                  12);
    p_thread_start(&t1);
    p_thread_init(&t2, "t2", t2_thread_entry, NULL,
                  t2_thread_stack,
                  sizeof(t2_thread_stack),
                  12);
    p_thread_start(&t2);
}
/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
#include <puppy.h>
#include <puppy/posix/unistd.h>
#include <puppy/posix/sched.h>
#include <puppy/posix/pthread.h>

static int end_exec;

#define NUM_THREADS 4

void *thread_func(void *arg)
{
    int thread_id = *(int *)arg;
    printk("Thread %d is running on core %d\n", thread_id, sched_getcpu());
    // Do some computation here
    
    printk("Thread %d begin sleep on core %d\n", thread_id, sched_getcpu());
    sleep(1);
    
    printk("Thread %d end sleep on core %d\n", thread_id, sched_getcpu());
    pthread_exit(0);
    return NULL;
}

int smp_testcase(void)
{
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    int i, rc;

    // Create threads
    for (i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        rc = pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
        if (rc) {
            printk("Error creating thread %d\n", i);
            return -1;
        }
    }

    // Wait for threads to finish
    for (i = 0; i < NUM_THREADS; i++) {
        rc = pthread_join(threads[i], NULL);
        if (rc) {
            printk("Error joining thread %d\n", i);
            return -1;
        }
    }
    printk("Test PASSED\n");

    return 0;
}

void test_smp(void)
{
    smp_testcase();
    P_TC_PASS();
}
P_TC_FUNC(test_smp, kernel.smp.tc);
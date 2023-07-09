/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

#define KLOG_TAG  "mem"
#define KLOG_LVL   KLOG_WARNING
#include <puppy/klog.h>

#ifdef ENABLE_TLSF
#include "tlsf.h"
#endif

static tlsf_t tlsf_ptr = 0;
static struct _sem_obj heap_sem;

void p_system_heap_init(void *begin_addr, size_t size)
{
    if (!tlsf_ptr)
    {
        tlsf_ptr = (tlsf_t)tlsf_create_with_pool(begin_addr, size);
        p_sem_init(&heap_sem, "heap_sem", 1, 1);
    }
    else
    {
        KLOG_ASSERT(0);
    }
}

void *p_malloc_align(size_t size, size_t align)
{
    void *ptr = NULL;

    if (tlsf_ptr)
    {
        p_sem_wait(&heap_sem);
        ptr = tlsf_memalign(tlsf_ptr, align, size);
        p_sem_post(&heap_sem);
    }
    return ptr;
}

void p_free_align(void *ptr)
{
    p_free(ptr);
}

void *p_malloc(size_t nbytes)
{
    void *ptr = NULL;

    if (tlsf_ptr)
    {
        p_sem_wait(&heap_sem);

        ptr = tlsf_malloc(tlsf_ptr, nbytes);

        p_sem_post(&heap_sem);
    }
    return ptr;
}

void p_free(void *ptr)
{
    if (tlsf_ptr)
    {
        p_sem_wait(&heap_sem);

        tlsf_free(tlsf_ptr, ptr);

        p_sem_post(&heap_sem);
    }
}

void *p_realloc(void *ptr, size_t nbytes)
{
    if (tlsf_ptr)
    {
        p_sem_wait(&heap_sem);

        ptr = tlsf_realloc(tlsf_ptr, ptr, nbytes);

        p_sem_post(&heap_sem);
    }
    return ptr;
}

void *p_calloc(size_t count, size_t size)
{
    void *ptr = NULL;
    size_t total_size;

    total_size = count * size;
    ptr = p_malloc(total_size);
    if (ptr != NULL)
    {
        /* clean memory */
        memset(ptr, 0, total_size);
    }

    return ptr;
}
static size_t used_mem = 0;
static size_t total_mem = 0;

static void mem_info(void *ptr, size_t size, int used, void *user)
{
    if (used)
    {
        used_mem += size;
    }
    total_mem += size;
}

void list_mem(void)
{
    uint8_t i = 0;
    uint8_t len = 0;

    used_mem = 0;
    total_mem = 0;

    tlsf_walk_pool(tlsf_get_pool(tlsf_ptr), mem_info, 0);
    printk("total memory: %d\n", total_mem);
    printk("used memory : %d\n", used_mem);
}
#ifdef ENABLE_NR_SHELL
void shell_free_cmd(char argc, char *argv)
{
    list_mem();
}
#include <nr_micro_shell.h>
NR_SHELL_CMD_EXPORT(free, shell_free_cmd);
#endif

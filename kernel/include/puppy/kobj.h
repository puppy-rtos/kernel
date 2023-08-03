/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* 
 * thread_obj def
 */
struct _thread_obj
{
    struct p_obj kobj;
    uint8_t      state;
    uint8_t      prio;
    uint8_t      mode;
    uint8_t      preved;
    void        *stack_addr;
    size_t       stack_size;
    p_tick_t     slice_ticks;
    p_node_t     tnode;

    void        *entry;
    void        *param;
    int          errno;
    void        *kernel_stack;

    p_timeout_t  timeout;
    uint8_t      bindcpu;
    uint8_t      oncpu;

    /** arch-specifics */
    void *arch_data;
};
void p_thread_init(p_obj_t obj, const char *name,
                                void (*entry)(void *param),
                                void    *param,
                                void    *stack_addr,
                                uint32_t stack_size,
                                uint8_t  prio,
                                uint8_t bindcpu);
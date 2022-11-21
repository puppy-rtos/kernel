/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

__attribute__((always_inline)) inline p_base_t arch_irq_lock(void)
{
    p_base_t key;

    __asm volatile("mrs %0, PRIMASK;"
        "cpsid i"
        : "=r" (key)
        :
        : "memory");

    return key;
}

__attribute__((always_inline)) inline void arch_irq_unlock(p_base_t key)
{
    if (key != 0U) {
        return;
    }
    __asm volatile(
        "cpsie i;"
        "isb"
        : : : "memory");
}

__attribute__((always_inline)) inline bool arch_irq_unlocked(p_base_t key)
{
    return key == 0U;
}

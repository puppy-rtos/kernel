/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <stdatomic.h>

#ifndef P_ARCH_CORTEX_M0
p_weak void arch_spin_lock_init(arch_spinlock_t *lock)
{
    atomic_flag_clear(&lock->flag);
}

p_weak __attribute__((always_inline)) inline void arch_spin_lock(arch_spinlock_t *lock)
{
    while (atomic_flag_test_and_set(&lock->flag)) {
        /* busy-wait */
    }
}

p_weak __attribute__((always_inline)) inline void arch_spin_unlock(arch_spinlock_t *lock)
{
    atomic_flag_clear(&lock->flag);
}

#else
#include <pico/lock_core.h>

void arch_spin_lock_init(arch_spinlock_t *lock)
{
    static uint8_t spin_cnt = 0;

    if ( spin_cnt < 32)
    {
        lock->slock = (uint32_t)spin_lock_instance(spin_cnt);
        spin_cnt = spin_cnt + 1;
    }
    else
    {
        lock->slock = 0;
    }
}

void arch_spin_lock(arch_spinlock_t *lock)
{
    if ( lock->slock != 0 )
    {
        spin_lock_unsafe_blocking((spin_lock_t*)lock->slock);
    }
}

void arch_spin_unlock(arch_spinlock_t *lock)
{
    if ( lock->slock != 0 )
    {
        spin_unlock_unsafe((spin_lock_t*)lock->slock);
    }
}
#endif

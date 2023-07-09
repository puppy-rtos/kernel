/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
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
__attribute__((always_inline)) inline bool arch_in_irq(void)
{
    volatile int tmp = 0;
    __asm volatile("mrs %0, IPSR;"
        : "=r" (tmp)
        :
        : "memory");
    return (tmp & 0x1f) != 0U;                 
}
// void HardFault_Handler(void)
// {
//   /* USER CODE BEGIN HardFault_IRQn 0 */
//   list_thread();
//   /* USER CODE END HardFault_IRQn 0 */
//   while (1)
//   {
//     /* USER CODE BEGIN W1_HardFault_IRQn 0 */
//     /* USER CODE END W1_HardFault_IRQn 0 */
//   }
// }
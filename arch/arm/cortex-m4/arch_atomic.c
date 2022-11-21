/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>

/**
  \brief   LDR Exclusive (32 bit)
  \details Executes a exclusive LDR instruction for 32 bit values.
  \param [in]    ptr  Pointer to data
  \return        value of type uint32_t at (*ptr)
 */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) /* ARM Compiler V6 */
#define __LDREXW        (p_atomic_t)__builtin_arm_ldrex
#elif defined(__ARMCC_VERSION)          /* ARM Compiler V5 */
#if __ARMCC_VERSION < 5060020
    #define __LDREXW(ptr)                                                        ((p_atomic_t ) __ldrex(ptr))
#else
    #define __LDREXW(ptr)          _Pragma("push") _Pragma("diag_suppress 3731") ((p_atomic_t ) __ldrex(ptr))  _Pragma("pop")
#endif
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
_Pragma("inline=forced") __intrinsic p_atomic_t __LDREXW(p_atomic_t volatile *ptr)
{
    return __LDREX((unsigned long *)ptr);
}
#elif defined (__GNUC__)                /* GNU GCC Compiler */
__attribute__((always_inline)) static inline  p_atomic_t __LDREXW(p_atomic_t *addr)
{
    p_atomic_t result;

    __asm volatile ("ldrex %0, %1" : "=r" (result) : "Q" (*addr) );
    return(result);
}
#endif

/**
  \brief   STR Exclusive (32 bit)
  \details Executes a exclusive STR instruction for 32 bit values.
  \param [in]  value  Value to store
  \param [in]    ptr  Pointer to location
  \return          0  Function succeeded
  \return          1  Function failed
 */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) /* ARM Compiler V6 */
#define __STREXW        (p_atomic_t)__builtin_arm_strex
#elif defined(__ARMCC_VERSION)          /* ARM Compiler V5 */
#if __ARMCC_VERSION < 5060020
    #define __STREXW(value, ptr)                                                 __strex(value, ptr)
#else
    #define __STREXW(value, ptr)   _Pragma("push") _Pragma("diag_suppress 3731") __strex(value, ptr)        _Pragma("pop")
#endif
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
_Pragma("inline=forced") __intrinsic p_atomic_t __STREXW(p_atomic_t value, p_atomic_t volatile *ptr)
{
    return __STREX(value, (unsigned long *)ptr);
}
#elif defined (__GNUC__)                /* GNU GCC Compiler */
__attribute__((always_inline)) static inline  p_atomic_t __STREXW(p_atomic_t value, p_atomic_t *addr)
{
    p_atomic_t result;

    __asm volatile ("strex %0, %2, %1" : "=&r" (result), "=Q" (*addr) : "r" (value) );
    return(result);
}
#endif

p_atomic_t arch_atomic_cas(p_atomic_t *ptr, p_atomic_t oldval, p_atomic_t newval)
{
    p_atomic_t result;

    do {
        result = __LDREXW(ptr);
        if (result != oldval)
        {
            break;
        }
    }while((__STREXW(newval, ptr)) != 0U);

    return result;
}

void arch_atomic_add(p_atomic_t *ptr, p_atomic_t val)
{
    p_atomic_t oldval;
    do {
        oldval = __LDREXW(ptr);
    }while((__STREXW(oldval + val, ptr)) != 0U);
}

void arch_atomic_sub(p_atomic_t *ptr, p_atomic_t val)
{
    p_atomic_t oldval;
    do {
        oldval = __LDREXW(ptr);
    }while((__STREXW(oldval - val, ptr)) != 0U);
}

void arch_atomic_or(p_atomic_t *ptr, p_atomic_t val)
{
    p_atomic_t oldval;
    do {
        oldval = __LDREXW(ptr);
    }while((__STREXW(oldval | val, ptr)) != 0U);
}

void arch_atomic_xor(p_atomic_t *ptr, p_atomic_t val)
{
    p_atomic_t oldval;
    do {
        oldval = __LDREXW(ptr);
    }while((__STREXW(oldval ^ val, ptr)) != 0U);
}

void arch_atomic_and(p_atomic_t *ptr, p_atomic_t val)
{
    p_atomic_t oldval;
    do {
        oldval = __LDREXW(ptr);
    }while((__STREXW(oldval & val, ptr)) != 0U);
}

void arch_atomic_nand(p_atomic_t *ptr, p_atomic_t val)
{
    p_atomic_t oldval;
    do {
        oldval = __LDREXW(ptr);
    }while((__STREXW(~(oldval & val), ptr)) != 0U);
}
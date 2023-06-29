/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
#include <puppy.h>

static void ktype_tc(void)
{
    int8_t     i8_value;     /**<  8bit integer type */
    int16_t    i16_value;    /**< 16bit integer type */
    int32_t    i32_value;    /**< 32bit integer type */
    int64_t    i64_value;    /**< 64bit integer type */
    uint8_t    u8_value;     /**<  8bit unsigned integer type */
    uint16_t   u16_value;    /**< 16bit unsigned integer type */
    uint32_t   u32_value;    /**< 32bit unsigned integer type */
    uint64_t   u64_value;    /**< 64bit unsigned integer type */
    size_t     s_value;      /**< Type for size number */
    ssize_t    ss_value;     /**< Used for a count of bytes or an error indication */

    P_UNUSED(i8_value);
    P_UNUSED(i16_value);
    P_UNUSED(i32_value);
    P_UNUSED(i64_value);
    P_UNUSED(u8_value);
    P_UNUSED(u16_value);
    P_UNUSED(u32_value);
    P_UNUSED(u64_value);
    P_UNUSED(s_value);
    P_UNUSED(ss_value);
}

static void kdef_tc(void)
{
    uint8_t value_true = true;
    uint8_t value_false = false;


    P_UNUSED(value_true);
    P_UNUSED(value_false);
}
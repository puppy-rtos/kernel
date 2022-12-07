/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PUPPY_INC_KLOG_H__
#define PUPPY_INC_KLOG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* KLOG level */
#define KLOG_ERROR           0
#define KLOG_WARNING         1
#define KLOG_INFO            2
#define KLOG_LOG             3

#ifndef KLOG_TAG
#define KLOG_TAG    "KLOG"
#endif

#ifdef KLOG_ENABLE

#ifndef KLOG_LVL
#define KLOG_LVL         KLOG_WARNING
#endif

/*
 * The color for terminal (foreground)
 * BLACK    30
 * RED      31
 * GREEN    32
 * YELLOW   33
 * BLUE     34
 * PURPLE   35
 * CYAN     36
 * WHITE    37
 */
#ifdef KLOG_COLOR
#define _KLOG_COLOR(n)        printk("\033["#n"m")
#define _KLOG_LOG_HDR(lvl_name, color_n)                    \
    printk("\033["#color_n"m[" lvl_name "/" KLOG_TAG "] ")
#define _KLOG_LOG_X_END                                     \
    printk("\033[0m\n")
#else
#define _KLOG_COLOR(n)
#define _KLOG_LOG_HDR(lvl_name, color_n)                    \
    printk("[" lvl_name "/" KLOG_TAG "] ")
#define _KLOG_LOG_X_END                                     \
    printk("\n")
#endif /* KLOG_COLOR */

#define klog_line(lvl, color_n, fmt, ...)                   \
    do                                                      \
    {                                                       \
        _KLOG_LOG_HDR(lvl, color_n);                        \
        printk(fmt, ##__VA_ARGS__);                         \
        _KLOG_LOG_X_END;                                    \
    }                                                       \
    while (0)

#define klog_assert(EX)                                     \
    if (!(EX))                                              \
    {                                                       \
        _KLOG_LOG_HDR("A", 31);                             \
        printk("(%s) at %s:%d", #EX,                        \
                __FUNCTION__, __LINE__);                    \
        _KLOG_LOG_X_END;                                    \
        while(1);                                           \
    }

#define klog_raw(...)         printk(__VA_ARGS__);

#else
#define klog_line(lvl, color_n, fmt, ...)
#define klog_raw(...)
#define klog_assert(EX)
#endif /* KLOG_ENABLE */

#if (KLOG_LVL >= KLOG_LOG)
#define KLOG_D(fmt, ...)      klog_line("D", 0, fmt, ##__VA_ARGS__)
#else
#define KLOG_D(...)
#endif

#if (KLOG_LVL >= KLOG_INFO)
#define KLOG_I(fmt, ...)      klog_line("I", 32, fmt, ##__VA_ARGS__)
#else
#define KLOG_I(...)
#endif

#if (KLOG_LVL >= KLOG_WARNING)
#define KLOG_W(fmt, ...)      klog_line("W", 33, fmt, ##__VA_ARGS__)
#else
#define KLOG_W(...)
#endif

#if (KLOG_LVL >= KLOG_ERROR)
#define KLOG_E(fmt, ...)      klog_line("E", 31, fmt, ##__VA_ARGS__)
#else
#define KLOG_E(...)
#endif

#define KLOG_ASSERT(EX)       klog_assert(EX)
#define KLOG_RAW(...)         klog_raw(__VA_ARGS__)

#endif /* define(KLOG_ENABLE) */

#ifdef __cplusplus
}
#endif

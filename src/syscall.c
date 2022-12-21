/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <string.h>
#include <puppy/target.h>

#define KLOG_TAG  "syscall"
#define KLOG_LVL   KLOG_INFO
#include <puppy/klog.h>

int syscall_enter(int syscall_no)
{
    struct _thread_obj *_thread = p_thread_self();
    _thread->mode = P_THREAD_MODE_KENL;
    // KLOG_D("syscall enter,num:%d", syscall_no);
    KLOG_D("syscall_enter msp:%x", __get_MSP());
    
    return syscall_no;
}
int syscall_leave(void)
{
    struct _thread_obj *_thread = p_thread_self();
    _thread->mode = P_THREAD_MODE_USER;
    KLOG_D("syscall_leave msp:%x", __get_MSP());
    return 0;
}

void sys_log(char *log)
{
    if (arch_thread_in_kernel(p_thread_self()) == true)
    {
	    KLOG_I("%s", log);
    }
    else
    {
        arch_syscall(0xfd, log);
    }
}
void sys_log2(char *log, int a)
{
    if (arch_thread_in_kernel(p_thread_self()) == true)
    {
	    KLOG_I("%s,%d", log, a);
    }
    else
    {
        arch_syscall(2, log, a);
    }
}
void sys_log3(char *log, int a, int b)
{
    if (arch_thread_in_kernel(p_thread_self()) == true)
    {
	    KLOG_I("%s,%d,%d", log, a, b);
    }
    else
    {
        arch_syscall(3, log, a, b);
    }
}
void sys_log4(char *log, int a, int b, int c)
{
    if (arch_thread_in_kernel(p_thread_self()) == true)
    {
	    KLOG_I("%s,%d,%d,%d", log, a, b, c);
    }
    else
    {
        arch_syscall(4, log, a, b, c);
    }
}
void sys_log5(char *log, int a, int b, int c, int d)
{
    if (arch_thread_in_kernel(p_thread_self()) == true)
    {
	    KLOG_I("%s,%d,%d,%d,%d", log, a, b, c, d);
    }
    else
    {
        arch_syscall(5, log, a, b, c, d);
    }
}

void sys_log6(char *log, int a, int b, int c, int d, int e)
{
    if (arch_thread_in_kernel(p_thread_self()) == true)
    {
	    KLOG_I("%s,%d,%d,%d,%d,%d", log, a, b, c, d, e);
    }
    else
    {
        arch_syscall(6, log, a, b, c, d, e);
    }
}

int sys_add(int a, int b)
{
    if (arch_thread_in_kernel(p_thread_self()) == true)
    {
	    return a + b;
    }
    else
    {
        return arch_syscall(7, a, b);
    }
}

void sys_default(int syscall_no)
{
	KLOG_D("default syscall");
}

void *p_syscall_get_api(int syscall_no)
{
    if (syscall_no == 0xfd)
	    return sys_log;
    if (syscall_no == 1)
	    return sys_log;
    if (syscall_no == 2)
	    return sys_log2;
    if (syscall_no == 3)
	    return sys_log3;
    if (syscall_no == 4)
	    return sys_log4;
    if (syscall_no == 5)
	    return sys_log5;
    if (syscall_no == 6)
	    return sys_log6;
    if (syscall_no == 7)
	    return sys_add;
    else
        return sys_default;
}

struct _thread_obj svc_test_t;
__attribute__((aligned(4)))
uint8_t svc_test_thread_stack[1024];

void svc_test_thread_entry(void *parm)
{
    char log[] = "svc_test in user";
    for(int i = 0; i < 1000; i++)
    {
        sys_log(log);
        sys_log2(log, 1);
        sys_log3(log, 1, 2);
        sys_log4(log, 1, 2, 3);
        sys_log5(log, 1, 2, 3, 4);
        sys_log6(log, 1, 2, 3, 4, 5);
        int ret = sys_add(5, 6);
        if (ret == (5 + 6))
        {
            KLOG_I("add in user ok!");
        }
        p_thread_sleep(1);
    }
}
void sys_log_usr(char *log)
{
    p_thread_init_user(&svc_test_t, "svc_text", svc_test_thread_entry, NULL,
                  svc_test_thread_stack,
                  sizeof(svc_test_thread_stack),
                  11);
    p_thread_start(&svc_test_t);
    for(int i = 0; i < 1000; i++)
    {
        sys_log(log);
        sys_log2(log, 1);
        sys_log3(log, 1, 2);
        sys_log4(log, 1, 2, 3);
        sys_log5(log, 1, 2, 3, 4);
        sys_log6(log, 1, 2, 3, 4, 5);
        int ret = sys_add(5, 6);
        if (ret == (5 + 6))
        {
            KLOG_I("add ok!");
        }
        p_thread_sleep(1);
    }
}

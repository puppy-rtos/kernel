/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <string.h>

#define KLOG_TAG  "syscall"
#define KLOG_LVL   KLOG_LOG
#include <puppy/klog.h>

int syscall_enter(int syscall_no)
{
    struct _thread_obj *_thread = p_thread_self();
    _thread->mode = P_THREAD_MODE_KENL;
	// KLOG_D("syscall enter,num:%d", syscall_no);
    
    return syscall_no;
}
int syscall_leave(void)
{
    struct _thread_obj *_thread = p_thread_self();
    _thread->mode = P_THREAD_MODE_USER;
    return 0;
}

void sys_log(char *log)
{
    if (arch_thread_in_kernel(p_thread_self()) == true)
    {
	    KLOG_D("%s", log);
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
	    KLOG_D("%s,%d", log, a);
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
	    KLOG_D("%s,%d,%d", log, a, b);
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
	    KLOG_D("%s,%d,%d,%d", log, a, b, c);
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
	    KLOG_D("%s,%d,%d,%d,%d", log, a, b, c, d);
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
	    KLOG_D("%s,%d,%d,%d,%d,%d", log, a, b, c, d, e);
    }
    else
    {
        arch_syscall(6, log, a, b, c, d, e);
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
        p_thread_sleep(2);
    }
}
void sys_log_usr(char *log)
{
    p_thread_init_user(&svc_test_t, "shell", svc_test_thread_entry, NULL,
                  svc_test_thread_stack,
                  sizeof(svc_test_thread_stack),
                  12);
    p_thread_start(&svc_test_t);
    for(int i = 0; i < 1000; i++)
    {
        sys_log(log);
        sys_log2(log, 1);
        sys_log3(log, 1, 2);
        sys_log4(log, 1, 2, 3);
        sys_log5(log, 1, 2, 3, 4);
        sys_log6(log, 1, 2, 3, 4, 5);
        p_thread_sleep(2);
    }
}

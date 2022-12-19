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
	KLOG_D("syscall enter,num:%d", syscall_no);
    return syscall_no;
}

void sys_log(char *log)
{
	KLOG_D("%s", log);
}
void sys_log2(char *log, int a)
{
	KLOG_D("%s,%d", log, a);
}
void sys_log3(char *log, int a, int b)
{
	KLOG_D("%s,%d,%d", log, a, b);
}
void sys_log4(char *log, int a, int b, int c)
{
	KLOG_D("%s,%d,%d,%d", log, a, b, c);
}
void sys_log5(char *log, int a, int b, int c, int d)
{
	KLOG_D("%s,%d,%d,%d,%d", log, a, b, c, d);
}
void sys_log6(char *log, int a, int b, int c, int d, int e)
{
	KLOG_D("%s,%d,%d,%d,%d,%d", log, a, b, c, d, e);
}


void sys_default(int syscall_no)
{
	KLOG_D("default syscall");
}

void *syscall_get_api(int syscall_no)
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

void sys_log_usr(char *log)
{
    arch_syscall(0xfd, log);
    arch_syscall(2, log, 1);
    arch_syscall(3, log, 1, 2);
    arch_syscall(4, log, 1, 2, 3);
    arch_syscall(5, log, 1, 2, 3, 4);
    arch_syscall(6, log, 1, 2, 3, 4, 5);
}

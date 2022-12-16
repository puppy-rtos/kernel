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

void sys_default(int syscall_no)
{
	KLOG_D("default syscall");
}

void *syscall_get_api(int syscall_no)
{
    if (syscall_no == 0xfd)
	    return sys_log;
    else
        return sys_default;
}

void sys_log_usr(char *log)
{
    arch_syscall(0xfd, log);
}

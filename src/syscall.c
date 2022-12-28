/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <string.h>
#include <puppy/target.h>
#include <puppy/syscall.h>

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
        arch_syscall(_NRSYS_LOG, log);
    }
}

int sys_test(char *log, int a, int b, int c, int d, int e)
{
    if (arch_thread_in_kernel(p_thread_self()) == true)
    {
	    KLOG_I("%s,%d,%d,%d,%d,%d", log, a, b, c, d, e);
        return a + b + c + d + e;
    }
    else
    {
        return arch_syscall(_NRSYS_SYSCALL_NR, log, a, b, c, d, e);
    }
}

void sys_default(int syscall_no)
{
	KLOG_D("default syscall");
}

#ifdef P_USERSPACE
#undef p_thread_init
#define p_thread_init       sys_thread_init
void sys_thread_init(void *arg[])
{
    k_thread_init(arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6]);
}
#endif

#define NRSYS(x) (void *)p_##x,
const static void* func_table[] =
{
    sys_log,
    #include <puppy/syscall_no.h>
    sys_test
};

void *p_syscall_get_api(int syscall_no)
{
    const void *func = (const void *)sys_default;

    if (syscall_no < sizeof(func_table) / sizeof(func_table[0]))
    {
        func = func_table[syscall_no];
    }

    return func;
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
        int ret = sys_test(log, 1, 2, 3, 4, 5);
        if (ret == (15))
        {
            KLOG_I("user test ok!");
        }
        p_thread_sleep(1);
    }
}
void svc_test(char *log)
{
    p_thread_init_user(&svc_test_t, "svc_text", svc_test_thread_entry, NULL,
                  svc_test_thread_stack,
                  sizeof(svc_test_thread_stack),
                  11);
    p_thread_start(&svc_test_t);
    for(int i = 0; i < 1000; i++)
    {
        sys_log(log);
        int ret = sys_test(log, 1, 2, 3, 4, 5);
        if (ret == (15))
        {
            KLOG_I("test ok!");
        }
        p_thread_sleep(1);
    }
}
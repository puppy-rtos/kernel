/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __PUPPY_H__
#define __PUPPY_H__

#include <puppy/default_config.h>
#include <puppy/toolchan.h>
#include <puppy/util.h>
#include <puppy/atomic.h>

#define P_UNUSED(x)                   ((void)x)

/* Puppy-RTOS object definitions */
typedef size_t            p_ubase_t;
typedef ssize_t           p_base_t;
typedef void             *p_obj_t;
typedef p_ubase_t         p_tick_t;
typedef volatile p_base_t p_atomic_t;    /**< Type for atomic */

/* Puppy-RTOS error code definitions */
#define P_EOK           0 /* There is no error */
#define P_ERROR         1 /* A unknow error happens */
#define P_EINVAL        2 /* Invalid argument */
#define P_EISR          3 /* Can't use in ISR */
#define P_ETIMEOUT      4 /* Timed out */
#define P_ENOMEM        5 /* No memory */
#define P_EBUSY         6 /**< Busy */
#define P_ENOSYS        7 /* No system */
#define P_ESYSCALL      8 /* Interrupted system call */

/**
 * @ingroup BasicDef
 *
 * @def P_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. P_ALIGN(13, 4)
 * would return 16.
 */
#define P_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

/**
 * @ingroup BasicDef
 *
 * @def P_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. P_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define P_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

void puppy_init(void);

/**
 * erron api
 */
int p_get_errno(void);
void p_set_errno(int errno);
const char *p_errno_str(int errno);

/**
 * object api
 */
#define P_CTL_CMD_LOCK     0x00
#define P_CTL_CMD_IIMELOCK 0x01
#define P_CTL_CMD_UNLOCK   0x02

typedef int (*p_ctl_t) (p_obj_t obj, int cmd, void *args);

#define P_OBJ_NAME_MAX 8

#define P_OBJ_TYPE_THREAD   0x01
#define P_OBJ_TYPE_IPC      0x02
#define P_OBJ_TYPE_MASK     0x007F
#define P_OBJ_TYPE_STATIC   0x0080

/* IPC */
#define P_OBJ_TYPE_IPC_SEM  0x01

// #define P_OBJ_SHARE_NONE    0x00;
// #define P_OBJ_SHARE_GLOBLE  0x01;
// #define P_OBJ_SHARE_GROUP   0x02;
struct p_obj
{
    const char *name;
    uint16_t    type;    /* The type of this object, 0-7:type, 8-15:user extern */
    // uint8_t     share;   /* The sharing type of this object */
    uint8_t     flag;    /* The support sharing type flag of this object */
    p_obj_t     owner;   /* A thread that owns this obj */
    p_ctl_t     control; /* The control func for this obj */
    p_node_t    node;    /* May be linked kernel:k_obj_list or thread:t_obj_list or group:g_obj_list */
};

void p_obj_init(p_obj_t obj, const char *name, uint8_t type, uint8_t ex_type);
p_obj_t p_obj_find(const char *name);
uint8_t p_obj_get_type(p_obj_t obj);
uint8_t p_obj_get_extype(p_obj_t obj);
p_obj_t p_obj_ioctl(p_obj_t obj, int cmd, void *args);
void p_obj_deinit(p_obj_t obj);

#define P_OBJ_LOCK(obj)              p_obj_ioctl(obj, P_CTL_CMD_LOCK, NULL);
#define P_OBJ_UNLOCK(obj)            p_obj_ioctl(obj, P_CTL_CMD_UNLOCK, NULL);
#define P_OBJ_LOCKED(obj)            p_obj_ioctl(obj, P_CTL_CMD_LOCKED, NULL);
#define P_OBJ_TIMELOCK(obj, timeout) p_obj_ioctl(obj, P_CTL_CMD_TIMELOCK, (void*)timeout));

p_base_t arch_irq_lock(void);
void arch_irq_unlock(p_base_t key);
bool arch_irq_locked(p_base_t key);
void arch_swap(unsigned int key);

/*
 * atomic interfaces
 */
void arch_atomic_add(p_atomic_t *ptr, p_atomic_t val);
void arch_atomic_sub(p_atomic_t *ptr, p_atomic_t val);
void arch_atomic_or(p_atomic_t *ptr, p_atomic_t val);
void arch_atomic_xor(p_atomic_t *ptr, p_atomic_t val);
void arch_atomic_and(p_atomic_t *ptr, p_atomic_t val);
void arch_atomic_nand(p_atomic_t *ptr, p_atomic_t val);
p_atomic_t arch_atomic_cas(p_atomic_t *ptr, p_atomic_t oldval, p_atomic_t newval);

/**
 * tick api
 */
void p_tick_init(int tick_persec);
void p_tick_inc(void);
p_tick_t p_tick_get(void);
int p_tick_persec(void);

/**
 * thread api
 */
#define P_THREAD_STATE_INIT        0x00
#define P_THREAD_STATE_SLEEP       0x01
#define P_THREAD_STATE_BLOCK       0x02
#define P_THREAD_STATE_READY       0x03
#define P_THREAD_STATE_RUN         0x04
#define P_THREAD_STATE_DEAD        0x05

#define P_THREAD_MODE_KENL         0x00
#define P_THREAD_MODE_USER         0x01

#define P_THREAD_PRIO_MAX          0xFF

typedef struct p_thread_attr
{
    const char *name;
    size_t      stack_size;
    p_tick_t    tick;
    uint8_t     priority; 
    uint8_t     reserved[3];
} p_thread_attr_t;

typedef void (*p_timeout_func) (p_obj_t obj, void *param);

typedef struct timeout
{
    p_base_t         tick;
    p_timeout_func   func;
    void            *param;
    p_node_t         node;
}p_timeout_t;

struct _thread_obj
{
    struct p_obj kobj;
    uint8_t      state;
    uint8_t      prio;
    uint8_t      mode;
    uint8_t      preved;
    void        *stack_addr;
    size_t       stack_size;
    p_tick_t     slice_ticks;
    p_node_t     tnode;

    void        *entry;
    void        *param;
    int          errno;
    void        *kernel_stack;

    p_timeout_t  timeout;
    /** arch-specifics: must always be at the end */
    struct arch_thread *arch;
};

void p_thread_init(p_obj_t obj, const char *name,
                                void (*entry)(void *param),
                                void    *param,
                                void    *stack_addr,
                                uint32_t stack_size,
                                uint8_t  prio);

p_obj_t p_thread_create(const char *name,
                        void (*entry)(void *parameter),
                        void    *parameter,
                        uint32_t stack_size,
                        uint8_t  prio);
int p_thread_start(p_obj_t obj);
int p_thread_yield(void);
int p_thread_block(p_obj_t obj);
int p_thread_wakeup(p_obj_t obj);
int p_thread_sleep(p_tick_t tick);
int p_thread_control(p_obj_t obj, int cmd, void *argv);
int p_thread_getattr(p_obj_t obj, p_thread_attr_t *attr);
/* todo: int p_thread_delete(p_obj_t obj); */
p_obj_t p_thread_self(void);
int p_thread_abort(p_obj_t obj);
void list_thread(void);

void thread_timeout_cb(p_base_t tick);     
int p_thread_set_timeout(p_tick_t timeout, p_timeout_func func, void *param);                     
void p_thread_entry(void (*entry)(void *parameter), void *param);
void arch_new_thread(struct _thread_obj *thread,
                            void    *stack_addr,
                            uint32_t stack_size);
/**
 * sched api
 */

int p_sched(void);
int p_sched_ready_insert(p_obj_t thread);
int p_sched_ready_remove(p_obj_t thread);
void p_sched_swap_out_cb(p_obj_t thread);
void p_sched_swap_in_cb(p_obj_t thread);

/**
 * sem api
 */
struct _sem_obj
{
    struct p_obj kobj;
    uint16_t    value;
    uint16_t    max_value;
    p_list_t    blocking_list;
};
void p_sem_init(p_obj_t obj, const char *name,
                     uint32_t    init_value,
                     uint32_t    max_value);
p_obj_t p_sem_create(const char *name,
                     uint32_t    init_value,
                     uint32_t    max_value);
int p_sem_post(p_obj_t obj);
int p_sem_timewait(p_obj_t obj, p_tick_t tick);
int p_sem_wait(p_obj_t obj);
int p_sem_control(p_obj_t obj, int cmd, void *argv);
int p_sem_delete(p_obj_t obj);


#include <puppy/util.h>

#endif

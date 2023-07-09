
#include <puppy.h>
#include <puppy/posix/unistd.h>
#include <puppy/posix/pthread.h>
#include <string.h>

const pthread_attr_t pthread_default_attr =
{
    P_SCHED_PRIO_DEFAULT,
    0, 0,     
    PTHREAD_CREATE_JOINABLE,    /* detach state */
    0,
    P_CONFIG_PTHREAD_STACK_DEFAULT, 
};

struct _pthread_data
{
    pthread_attr_t attr;
    struct _thread_obj tid;
    void *alloc_stack;

    pthread_func_t thread_entry;
    void *thread_parameter;

    /* return value */
    void *return_value;

    /* semaphore for joinable thread */
    struct _sem_obj joinable_sem;

    /* cancel state and type */
    uint8_t cancelstate;
    volatile uint8_t canceltype;
    volatile uint8_t canceled;

    // _pthread_cleanup_t *cleanup;
    void** tls; /* thread-local storage area */
};
typedef struct _pthread_data _pthread_data_t;

pthread_t _pthread_data_create(void)
{
    int index;
    _pthread_data_t *ptd = NULL;

    ptd = (_pthread_data_t*)p_malloc(sizeof(_pthread_data_t));
    if (!ptd) return NULL;

    memset(ptd, 0x0, sizeof(_pthread_data_t));
    ptd->canceled = 0;
    ptd->cancelstate = PTHREAD_CANCEL_DISABLE;
    ptd->canceltype = PTHREAD_CANCEL_DEFERRED;

    return ptd;
}
void _pthread_data_destroy(_pthread_data_t *ptd)
{
    p_free(ptd);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   pthread_startroutine_t startroutine, pthread_addr_t arg)
{
    char name[P_NAME_MAX];
    _pthread_data_t *ptd;
    void *stack;
    static uint16_t pthread_number = 0;
    /* allocate posix thread data */
    ptd = _pthread_data_create();

    if (attr != NULL)
    {
        ptd->attr = *attr;
    }
    else
    {
        /* use default attribute */
        ptd->attr = pthread_default_attr;
    }
    if (ptd->attr.detachstate == PTHREAD_CREATE_JOINABLE)
    {
        p_sem_init(&ptd->joinable_sem, "sem", 0, 1);
    }
     /* set parameter */
    ptd->thread_entry = startroutine;
    ptd->thread_parameter = arg;

    /* stack */
    if (ptd->attr.stackaddr == 0)
    {
        stack = (void *)p_malloc(ptd->attr.stacksize);
        ptd->alloc_stack = stack;
    }
    else
    {
        stack = (void *)(ptd->attr.stackaddr);
    }
    /* initial this pthread to system */
    snprintf(name, sizeof(name), "pth%02d", pthread_number ++);
    p_thread_init(&ptd->tid, name, (void (*)(void *))startroutine, arg,
                       stack, ptd->attr.stacksize,
                       ptd->attr.priority);
    *thread = ptd;
        /* start thread */
    if (p_thread_start(&ptd->tid) == 0)
        return 0;

    return 0;
}

int pthread_join(pthread_t thread, pthread_addr_t *value)
{
    _pthread_data_t *ptd;
    int result;

    ptd = thread;

    if (ptd == NULL)
    {
        return -1; /* invalid pthread id */
    }

    if (ptd->attr.detachstate == PTHREAD_CREATE_DETACHED)
    {
        return -1; /* join on a detached pthread */
    }

    result = p_sem_wait(&ptd->joinable_sem);
    if (result == 0)
    {
        /* get return value */
        if (value != NULL)
            *value = ptd->return_value;

        p_thread_abort(&ptd->tid);
        if (ptd->alloc_stack)
        {
            p_free(ptd->alloc_stack);
        }
        p_obj_deinit(&ptd->tid);

        /* destroy this pthread */
        _pthread_data_destroy(ptd);
    }
    else
    {
        return -1;
    }

    return 0;
}

void pthread_exit(pthread_addr_t value)
{
    _pthread_data_t *ptd;
    ptd = p_container_of(p_thread_self(), _pthread_data_t, tid);
    p_sem_post(&ptd->joinable_sem);
}

unsigned sleep(unsigned int __seconds)
{   
    p_thread_sleep(100 * __seconds);
}
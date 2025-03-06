
#include <puppy_core.h>

#include <stdint.h>
#include <stddef.h>

int main( void )
{
    int puppy_board_init(void);
    puppy_board_init();
    puppy_init();
    return 0;
}

static int end_exec;
static void *a_thread_func()
{
    end_exec = 1;
    PUP_PRINTK("Wait for 3 seconds for thread to finish execution:\n");

    for (int i = 0; i < 3; i++)
    {
        sched_yield();
        PUP_PRINTK("Thread is running\n");
    }

    return NULL;
}
char pth1_stack[4096];
char pth2_stack[4096];
static int posix_testcase(void)
{
    pthread_t new_th;
    /* Initialize flag */
    end_exec = 0;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 4096);
    pthread_attr_setstackaddr(&attr, pth1_stack);

    /* Create a new thread. */
    if (pthread_create(&new_th, &attr, a_thread_func, NULL) != 0) {
        PUP_PRINTK("Error creating thread1\n");
        return -2;
    }
    pthread_attr_setstackaddr(&attr, pth2_stack);
    if (pthread_create(&new_th, &attr, a_thread_func, NULL) != 0) {
        PUP_PRINTK("Error creating thread2\n");
        return -2;
    }

    /* Wait for thread to return */
    if (pthread_join(new_th, NULL) != 0) {
        PUP_PRINTK("Error in pthread_join()\n");
        return -2;
    }
    if (end_exec == 0) {
        PUP_PRINTK("Test FAILED: When using pthread_join(), "
               "main() did not wait for thread to finish "
               "execution before continuing.\n");
        return -1;
    }

    PUP_PRINTK("Test PASSED\n");
    return 0;
}

void *puppy_main_thread(void *arg) {
    posix_testcase();
    sched_yield();
	return 0;
}

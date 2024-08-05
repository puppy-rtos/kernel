
#include <puppy.h>

static void say_hello(void) {
    printk("hello! puppy-rtos!\n");
}
P_INIT_FUNC(say_hello);

int main( void ) {
    puppy_init();
    puppy_start();
	return 0;
}


#include <puppy.h>
#include "riscv-virt.h"

#define mainVECTOR_MODE_DIRECT	1

void trap_vector(void);
/*-----------------------------------------------------------*/

int main( void )
{
	int ret;
	// trap handler initialization
	#if( mainVECTOR_MODE_DIRECT == 1 )
	{
		__asm__ volatile( "csrw mtvec, %0" :: "r"( trap_vector ) );
	}
	#else
	{
		__asm__ volatile( "csrw mtvec, %0" :: "r"( ( uintptr_t )freertos_vector_table | 0x1 ) );
	}
	#endif

	// vSendString( "Hello Puppy!" );
	board_init();
	// trap_test();
	
	// printk("tick:%d\n", get_ticks());

    puppy_init();
    puppy_start();
	while (1)
	{
		/* code */
	}
	

	return 0;
}



#include <string.h>

#include "riscv-virt.h"

int xGetCoreID( void )
{
int id;

	__asm ("csrr %0, mhartid" : "=r" ( id ) );

	return id;
}


void handle_trap(void)
{
	while (1)
		;
}

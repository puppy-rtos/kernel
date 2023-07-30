

#include <string.h>

#include "riscv-virt.h"
#include "ns16550.h"

int xGetCoreID( void )
{
int id;

	__asm ("csrr %0, mhartid" : "=r" ( id ) );

	return id;
}

void vSendString( const char *s )
{
struct device dev;
size_t i;

	dev.addr = NS16550_ADDR;

	// portENTER_CRITICAL();

	for (i = 0; i < strlen(s); i++) {
		vOutNS16550( &dev, s[i] );
	}
	vOutNS16550( &dev, '\n' );

	// portEXIT_CRITICAL();
}

void handle_trap(void)
{
	while (1)
		;
}

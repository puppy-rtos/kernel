
#include <puppy.h>

#ifdef ENABLE_GCOV 
#include "gcov_public.h"
static void pkg_gcov_init(void){
#ifndef PICO_BIT_OPS_PICO
    __gcov_call_constructors();
#endif
}
P_INIT_FUNC(pkg_gcov_init);
#endif

int main( void )
{
    puppy_init();
    puppy_start();
	return 0;
}

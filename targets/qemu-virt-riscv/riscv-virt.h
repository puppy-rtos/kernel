
#ifndef RISCV_VIRT_H_
#define RISCV_VIRT_H_

#include "riscv-reg.h"

#ifdef __ASSEMBLER__
#define CONS(NUM, TYPE)NUM
#else
#define CONS(NUM, TYPE)NUM##TYPE
#endif /* __ASSEMBLER__ */

#define PRIM_HART			0

#define CLINT_ADDR			CONS(0x02000000, UL)
#define CLINT_MSIP			CONS(0x0000, UL)
#define CLINT_MTIMECMP		CONS(0x4000, UL)
#define CLINT_MTIME			CONS(0xbff8, UL)

#define NS16550_ADDR		CONS(0x10000000, UL)

#ifndef __ASSEMBLER__

int xGetCoreID( void );
void vSendString( const char * s );

#endif /* __ASSEMBLER__ */

#endif /* RISCV_VIRT_H_ */

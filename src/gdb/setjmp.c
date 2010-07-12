//m68k setjmp / longjmp assembler

#include "setjmp.h"

UINT32 tempA0;

asm(".text");
asm(".globl setjmp");
asm("setjmp:");

//move env the pointer to d0
asm("   movel %a0, (tempA0)");
asm("	movel 4(%sp), %a0");

//move return address to first value in D0, and increment D0
asm("	movel (%sp), (%a0)+");

//d0 not saved because it's the return value
asm("	movel %d1, (%a0)+");
asm("	movel %d2, (%a0)+");
asm("	movel %d3, (%a0)+");
asm("	movel %d4, (%a0)+");
asm("	movel %d5, (%a0)+");
asm("	movel %d6, (%a0)+");
asm("	movel %d7, (%a0)+");
asm("	movel (tempA0), (%a0)+");
asm("	movel %a1, (%a0)+");
asm("	movel %a2, (%a0)+");
asm("	movel %a3, (%a0)+");
asm("	movel %a4, (%a0)+");
asm("	movel %a5, (%a0)+");
asm("	movel %a6, (%a0)+");
asm("	movel %a7, (%a0)");
asm("	movel #0, %d0");
asm("	rts");

#define valueOffset 64

asm(".text");
asm(".globl longjmp");
asm("longjmp:");
//env pointer
asm("	movel 4(%sp), %a0");
//int value
asm("	movel 8(%sp), %d0");

asm("	movel 4(%a0), %d1");
asm("	movel 8(%a0), %d2");
asm("	movel 12(%a0), %d3");
asm("	movel 16(%a0), %d4");
asm("	movel 20(%a0), %d5");
asm("	movel 24(%a0), %d6");
asm("	movel 28(%a0), %d7");
//asm("	movel 32(%d0), %a0");
asm("	movel 36(%a0), %a1");
asm("	movel 40(%a0), %a2");
asm("	movel 44(%a0), %a3");
asm("	movel 48(%a0), %a4");
asm("	movel 52(%a0), %a5");
asm("	movel 56(%a0), %a6");
asm("	movel 60(%a0), %sp");
asm("	movel (%a0), (%sp)");
//how do I do restore SP AND fetch the return value from 8(%sp) INTO %d0? new value in struct
asm("  movel 32(%a0), %a0");
asm("	rts");

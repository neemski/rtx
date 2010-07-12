#ifndef SETJMP_H
#define SETJMP_H

#include "rtx_inc.h"
typedef struct jmp_buf_tag
{
	/* NOTE: The machine-dependent definitions of `__sigsetjmp'
	 assume that a `jmp_buf' begins with a `__jmp_buf' and that
	 `__mask_was_saved' follows it.  Do not move these members
	 or add others before it.  */
	//offset 0
	UINT32 spPtr; //the return value that the stack pointer was pointing to, (%sp)

	//offset 4 to 28
	UINT32 dregs[6]; //%d0 is NOT saved, since it is the return value

	//offset 32 to 60
	UINT32 aregs[7]; //including stack pointer

	//offset 64
	UINT32 tempRet; //used to clobber no more than 1 register
} jmp_buf_tag;

typedef jmp_buf_tag *jmp_buf;

int setjmp(jmp_buf);
void longjmp(jmp_buf, int);

#endif

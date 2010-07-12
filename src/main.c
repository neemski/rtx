/**
 * @file: hello.c
 * @brief: hello world sample code. Ouput to janusROM terminal
 * @author: ECE354 Lab Instructors and TAs
 * @author: Irene Huang
 * @date: 2010/05/03
 */
#include "dbug.h"
#include "kernel/kernel.h"
#include "rtx_api.h"
#include "kernel/kernel_globals.h"
/*
 * gcc expects this function to exist
 */
int __main(void)
{
	return 0;
}
/*
 * Entry point, check with m68k-coff-nm
 */
extern void __REGISTER_TEST_PROCS_ENTRY__();
int main(void)
{
	DISABLE_INTERRUPTS();
#ifdef _TEST_
	__REGISTER_TEST_PROCS_ENTRY__();
#endif
#ifdef _GDB_
#include "gdb_uart.h"
	gdb_uart_init();
	extern void set_debug_traps();
	set_debug_traps();
	BREAKPOINT();
#endif
#ifdef FOO
	void * start_of_heap = &_end;
	char *ptr = (char *) start_of_heap;
	while ((UINT32)ptr <= (UINT32) 0x10200000)
		*(ptr++) = 0xff;
#endif

	kernel_init();
	scheduler();
	printf("main complete - exiting");
	return 0;
}

/*
 * process.c
 *
 *  Created on: 2010-06-04
 *      Author: mzakharo
 */

#include "process.h"
#include "pcb.h"
#include "dbug.h"
#include "kernel_globals.h"
#include "priority_queue.h"
#include "rtx_api.h"
#include "config.h"
#include "rtx_test.h"
#include "sys_process.h"
#include "user_process.h"
#include "i_process.h"
#include "fixture.h"


#undef printf
#define printf(format, ...){rtx_dbug_outs((CHAR *) "rtx_kern: ");\
							printf(format, ## __VA_ARGS__); };

void null_proc();
typedef struct proc_member
{
	void * function;
	int priority;
	int pid;
	int stack_size;
	int procType;
} proc_member;

static proc_member proc_array[] =
{
#ifdef NULL_PROC
		{ null_proc, NULL_P, NULL_PID, 2048, SYS_PROC },
#endif
#ifdef  I_UART
		{ i_uart, I_PRIORITY, I_UART_PID, 4096, SYS_PROC },
#endif
#ifdef  I_TIMER0
		{ i_timer0, I_PRIORITY, I_TIMER0_PID, 4096, SYS_PROC },
#endif
#ifdef  KCD
		{ sys_keyboard_decoder, MEDIUM, SYS_KEYBOARD_DECODER_PID, 4096, SYS_PROC },
#endif
#ifdef CRT
		{ sys_crt_display, MEDIUM, SYS_CRT_DISPLAY_PID, 4096, SYS_PROC },
#endif
#ifdef WALL_CLOCK
		{ user_wall_clk_display, LOW, USER_WALL_CLK_DISPLAY_PID, 4096, USER_PROC },
#endif
#ifdef SET_PRIORITY
		{ user_set_priority, LOW, USER_SET_PRIORITY_PID, 4096, USER_PROC },
#endif
#ifdef USER_PROC_A
		{ user_process_a, LOW, USER_PROC_A_PID, 2048, USER_PROC },
#endif
#ifdef USER_PROC_B
		{ user_process_b, LOW, USER_PROC_B_PID, 2048, USER_PROC },
#endif
#ifdef USER_PROC_C
		{ user_process_c, MEDIUM, USER_PROC_C_PID, 2048, USER_PROC },
#endif
		};

static VOID setup_context(proc_member * member)
{
	pcb * newProc = kernel_malloc(sizeof(pcb));
	BYTE * stack = kernel_malloc(member->stack_size);
	printf("process PID %d '%s', priority %d, at address 0x%x", member->pid,
			pid_to_string(member->pid), member->priority,
			(int) member->function);

	if (newProc == NULL || stack == NULL)
	{
		printf("Kernel panic: out of memory for the processes");
		return;
	}
	newProc->pid = member->pid;
	newProc->priority = member->priority;

	init_rtx_queue(&newProc->mailBox);
	newProc->procType = member->procType;
	newProc->state = READY;

	*(UINT32*) ((UINT32) stack + member->stack_size - 4)
			= (UINT32) member->function;
	if (member->priority == I_PRIORITY)
		*(UINT32*) ((UINT32) stack + member->stack_size - 8) = 0x40000700;
	else
		*(UINT32*) ((UINT32) stack + member->stack_size - 8) = 0x40000000;
	newProc->stack_pointer = ((UINT32) stack + member->stack_size - 68);

	push(&state_q, newProc);
}
void init_processes()
{
	int i;
#ifdef _TEST_
	proc_member test;
	for (i = 0; i < NUM_TEST_PROCS; i++)
	{
		test.function = g_test_proc[i].entry;
		test.pid = (int) g_test_proc[i].pid;
		test.priority = (signed int) g_test_proc[i].priority;
		test.stack_size = g_test_proc[i].sz_stack;
		test.procType = USER_PROC;
		setup_context(&test);
	}
#endif
	for (i = 0; i < sizeof(proc_array) / sizeof(proc_member); i++)
    {
    	setup_context(&proc_array[i]);
    }
}

void null_proc()
{
	while (TRUE);
}

void __attribute__ ((section ("__REGISTER_RTX__"))) register_rtx()
{
	rtx_dbug_outs((CHAR *) "rtx: Entering register_rtx()\r\n");
	g_test_fixture.send_message = send_message;
	g_test_fixture.receive_message = receive_message;
	g_test_fixture.request_memory_block = request_memory_block;
	g_test_fixture.release_memory_block = release_memory_block;
	g_test_fixture.release_processor = release_processor;
	g_test_fixture.delayed_send = delayed_send;
	g_test_fixture.set_process_priority = set_process_priority;
	g_test_fixture.get_process_priority = get_process_priority;
#ifdef _INTERNAL_
	g_test_fixture.count_mem = count_mem;
	g_test_fixture.free = free;
	g_test_fixture.malloc = malloc;
#endif
	rtx_dbug_outs((CHAR *) "rtx: leaving register_rtx()\r\n");
}

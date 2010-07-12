/*
 * i_entry.c
 *
 *  Created on: 2010-05-28
 *      Author: mzakharo
 */
#include "api_entry.h"
#include "rtx_inc.h"
#include "kernel.h"
#include "kernel_globals.h"
#include "dbug.h"

VOID api_entry_handler(VOID);

asm(".text");
asm(".globl api_entry");
asm("api_entry:");
DISABLE_INTERRUPTS();
asm("move.l %d0, -(%a7)");
asm("move.l %d1, -(%a7)");
asm("move.l %d2, -(%a7)");
asm("move.l %d3, -(%a7)");
asm("move.l %d4, -(%a7)");
asm("move.l %d5, -(%a7)");
asm("move.l %d6, -(%a7)");
asm("move.l %d7, -(%a7)");
asm("move.l %a0, -(%a7)");
asm("move.l %a1, -(%a7)");
asm("move.l %a2, -(%a7)");
asm("move.l %a3, -(%a7)");
asm("move.l %a4, -(%a7)");
asm("move.l %a5, -(%a7)");
asm("move.l %a6, -(%a7)");
asm("move.l %a7, API_ENTRY_STACK");
asm("jmp api_entry_handler");

VOID api_entry_handler(VOID)
{
	UINT32 select;
	UINT32 param1;
	UINT32 param2;
	UINT32 param3;
	UINT32 ret = 0;
	register UINT32 d0 asm ("%d0");
	register UINT32 d1 asm ("%d1");
	register UINT32 d2 asm ("%d2");
	register UINT32 d3 asm ("%d3");
	select = d0;
	param1 = d1;
	param2 = d2;
	param3 = d3;

	switch (select)
	{
	case PROC_SWITCH_F:
		kernel_proc_switch((pcb *) param1);
		break;
	case MALLOC_F:
		ret = (UINT32) kernel_malloc(param1);
		break;
	case KERNEL_FREE_F:
		kernel_free((void*) param1);
		break;
	case REQ_MEMBLOCK_F:
		ret = (UINT32) kernel_request_memory_block();
		break;
	case RELEASE_MEMBLOCK_F:
		ret = kernel_release_memory_block((void*) param1);
		break;
	case SEND_MSG_F:
		ret = kernel_send_message(param1, (void *) param2, 0);
		break;
	case RCV_MSG_F:
		ret = (UINT32) kernel_receive_message((int *) param1);
		break;
	case RELEASE_PROCESSOR_F:
		kernel_release_processor();
		break;
	case HOTKEY_F:
		kernel_send_hotkey((BYTE) param1);
		break;
	case SEND_DELAYED_MSG_F:
		ret = kernel_delayed_send((int) param1, (void *) param2, (int) param3);
		break;
	case COUNT_MEM_BLKS_F:
		ret = kernel_count_memory((int) param1);
		break;
	case SET_PROCESS_PRIORITY:
		ret = kernel_set_process_priority((int) param1,(int)param2 );
		break;
	case GET_PROCESS_PRIORITY:
		ret = kernel_get_process_priority((int) param1);
		break;
	case GET_TICK_COUNT:
		ret = kernel_get_timer0_tick();
		break;
	default:
		printf("unknown API detected");
		break;
	}
	d0 = ret;
	asm("unlk %fp");
	asm("move.l (%a7)+, %a6");
	asm("move.l (%a7)+, %a5");
	asm("move.l (%a7)+, %a4");
	asm("move.l (%a7)+, %a3");
	asm("move.l (%a7)+, %a2");
	asm("move.l (%a7)+, %a1");
	asm("move.l (%a7)+, %a0");
	asm("move.l (%a7)+, %d7");
	asm("move.l (%a7)+, %d6");
	asm("move.l (%a7)+, %d5");
	asm("move.l (%a7)+, %d4");
	asm("move.l (%a7)+, %d3");
	asm("move.l (%a7)+, %d2");
	asm("move.l (%a7)+, %d1");
	asm("add.l #4, %a7");
	asm("rte");
}

/*
 * rtx_api.c
 *
 *  Created on: 2010-05-28
 *      Author: mzakharo
 */

/**
 * @param: pid process id of the process that will be switched to
 */
#include "api_entry.h"
#include "rtx_api.h"
#include "dbug.h"
#include "config.h"
void * malloc(size_t size)
{
	void * addr;
	register UINT32 d0 asm ("%d0");
	register UINT32 d1 asm ("%d1");
	/* Store registers */
	asm( "move.l %d0, -(%a7)" );
	asm( "move.l %d1, -(%a7)" );

	/* Load size into d1 */
	d1 = size;

	/* Setup trap function */
	d0 = MALLOC_F;
	asm( "trap #0" );
	addr = (void *) d0;
	/* Restore registers  */
	asm(" move.l (%a7)+, %d1" );
	asm(" move.l (%a7)+, %d0" );
	return addr;
}
void free(void* ptr)
{
	register UINT32 d0 asm ("%d0");
	register UINT32 d1 asm ("%d1");

	/* Store registers */
	asm( "move.l %d0, -(%a7)" );
	asm( "move.l %d1, -(%a7)" );

	/* Load ptr into d1 */
	d1 = (UINT32) ptr;
	/* Setup trap function */
	d0 = KERNEL_FREE_F;
	asm( "trap #0" );

	/* Restore registers  */
	asm(" move.l (%a7)+, %d1" );
	asm(" move.l (%a7)+, %d0" );

}

int send_message(int process_ID, void * MessageEnvelope)
{
	int success;
	register UINT32 d0 asm ("%d0");
	register UINT32 d1 asm ("%d1");
	register UINT32 d2 asm ("%d2");
	//rtx_dbug_outs("rtx: send message\n");
	/* Store registers */
	asm( "move.l %d0, -(%a7)" );
	asm( "move.l %d1, -(%a7)" );
	asm( "move.l %d2, -(%a7)" );

	/* Load process_ID into d1 */

	/* Setup trap function */
	d1 = process_ID;
	d2 = (UINT32)MessageEnvelope;
	d0 = SEND_MSG_F;
	asm( "trap #0" );
	success = d0;

	/* Restore registers  */
	asm(" move.l (%a7)+, %d2" );
	asm(" move.l (%a7)+, %d1" );
	asm(" move.l (%a7)+, %d0" );

	return success;
}

void * receive_message(int * sender_ID)
{
	void * msgPtr;
	register UINT32 d0 asm ("%d0");
	register UINT32 d1 asm ("%d1");
	//rtx_dbug_outs("rtx: receive message\n");

	/* Store registers */
	asm( "move.l %d0, -(%a7)" );
	asm( "move.l %d1, -(%a7)" );

	d1 = (UINT32 ) sender_ID;

	/* Setup trap function */
	d0 = RCV_MSG_F;
	asm( "trap #0" );
	msgPtr = (void *) d0;
	/* Restore registers  */
	asm(" move.l (%a7)+, %d1" );
	asm(" move.l (%a7)+, %d0" );
	return msgPtr;
}

void * request_memory_block()
{
	void * addr;
	register UINT32 d0 asm ("%d0");
	/* Store registers */
	asm( "move.l %d0, -(%a7)" );

	/* Setup trap function */
	d0 = REQ_MEMBLOCK_F;
	asm( "trap #0" );
	addr = (void *) d0;
	/* Restore registers  */
	asm(" move.l (%a7)+, %d0" );
	return addr;
}
int release_memory_block(void * MemoryBlock)
{
	int ret;
	register UINT32 d0 asm ("%d0");
	register UINT32 d1 asm ("%d1");
	//printf("rtx: release");

	/* Store registers */
	asm( "move.l %d0, -(%a7)" );
	asm( "move.l %d1, -(%a7)" );

	/* Load ptr into d1 */
	d1 = (UINT32 ) MemoryBlock;
	/* Setup trap function */
	d0 = RELEASE_MEMBLOCK_F;
	asm( "trap #0" );
	ret = (int) d0;
	/* Restore registers  */
	asm(" move.l (%a7)+, %d1" );
	asm(" move.l (%a7)+, %d0" );
	return ret;
}

/**************************************************************************
 * registers a function for a given interrupt vector
 **************************************************************************/
VOID exceptionHandler(BYTE interruptVector, VOID * address)
{
	UINT32 vector_base = 0x10000000;

	vector_base += 4 * interruptVector;

	asm( "move.l %d0, -(%a7)" );
	asm( "move.l %a1, -(%a7)" );
	asm( "move.l %0,%%d0" : :"m"(address) );
	asm( "move.l %0, %%a1" : :"m"(vector_base) );
	asm( "move.l %d0,(%a1)");
	asm( "move.l (%a7)+, %a1" );
	asm( "move.l (%a7)+, %d0" );
}

int release_processor()
{
	register UINT32 d0 asm ("%d0");
	/* Store registers */
	asm( "move.l %d0, -(%a7)" );

	/* Setup trap function */
	d0 = RELEASE_PROCESSOR_F;
	asm( "trap #0" );
	/* Restore registers  */
	asm(" move.l (%a7)+, %d0" );
	return 0;
}

void imr_enable(int bit)
{
	UINT32 mask;
	mask = SIM_IMR;
	mask &= 0x0003ffff & ~bit; //unmask uart0 interrupt
	SIM_IMR = mask;
}

void send_hotkey(BYTE hotkey)
{
	register UINT32 d0 asm ("%d0");
	register UINT32 d1 asm ("%d1");

	/* Store registers */
	asm( "move.l %d0, -(%a7)" );
	asm( "move.l %d1, -(%a7)" );

	d1 = (UINT32 ) hotkey;

	/* Setup trap function */
	d0 = HOTKEY_F;
	asm( "trap #0" );
	/* Restore registers  */
	asm(" move.l (%a7)+, %d1" );
	asm(" move.l (%a7)+, %d0" );
}

/*Timing Service*/
int delayed_send(int process_ID, void * MessageEnvelope, int delay)
{
	int success;
	//rtx_dbug_outs("rtx: delayed_send");
  	register UINT32 d0 asm ("%d0");
	register UINT32 d1 asm ("%d1");
	register UINT32 d2 asm ("%d2");
	register UINT32 d3 asm ("%d3");

	/* Store registers */
	asm( "move.l %d0, -(%a7)" );
	asm( "move.l %d1, -(%a7)" );
	asm( "move.l %d2, -(%a7)" );
	asm( "move.l %d3, -(%a7)" );

	d1 = (UINT32 ) process_ID;
	d2 = (UINT32 ) MessageEnvelope;
	d3 = (UINT32 ) delay;

	d0 = SEND_DELAYED_MSG_F;
	asm( "trap #0" );
	success = d0;

	/* Restore registers  */
	asm(" move.l (%a7)+, %d3" );
	asm(" move.l (%a7)+, %d2" );
	asm(" move.l (%a7)+, %d1" );
	asm(" move.l (%a7)+, %d0" );
	return success;
}

char * pid_to_string(int pid)
{
	switch (pid)
	{
	case NULL_PID:
		return "NULL_PID";
	case 1:
		return "TEST_1_PID";
	case 2:
		return "TEST_2_PID";
	case 3:
		return "TEST_3_PID";
	case 4:
		return "TEST_4_PID";
	case 5:
		return "TEST_5_PID";
	case 6:
		return "TEST_6_PID";
	case SYS_KEYBOARD_DECODER_PID:
		return "SYS_KEYBOARD_DECODER_PID";
	case SYS_CRT_DISPLAY_PID:
		return "SYS_CRT_DISPLAY_PID";
	case USER_WALL_CLK_DISPLAY_PID:
		return "WALL_CLOCK";
	case USER_SET_PRIORITY_PID:
		return "SET_PRIORITY_PID";
		break;
	case USER_PROC_A_PID:
		return "USER_PROC_A";
		break;
	case USER_PROC_B_PID:
		return "USER_PROC_B";
		break;
	case USER_PROC_C_PID:
		return "USER_PROC_C";
		break;
	case I_UART_PID:
		return "I_UART_PID";
	case I_TIMER0_PID:
		return "I_TIMER_PID";
	};
	return "UNknown PID";
}

int count_mem(int count_type)
{
	int ret;
	register UINT32 d0 asm ("%d0");
	register UINT32 d1 asm ("%d1");

	/* Store registers */
	asm( "move.l %d0, -(%a7)" );
	asm( "move.l %d1, -(%a7)" );

	d1 = (UINT32 ) count_type;

	/* Setup trap function */
	d0 = COUNT_MEM_BLKS_F;
	asm( "trap #0" );
	ret = (int) d0;
	/* Restore registers  */
	asm(" move.l (%a7)+, %d1" );
	asm(" move.l (%a7)+, %d0" );
	return ret;
}

int set_process_priority (int process_ID, int priority)
{
	int success;
	register UINT32 d0 asm ("%d0");
	register UINT32 d1 asm ("%d1");
	register UINT32 d2 asm ("%d2");
	/* Store registers */
	asm( "move.l %d0, -(%a7)" );
	asm( "move.l %d1, -(%a7)" );
	asm( "move.l %d2, -(%a7)" );

	d1 = (UINT32) process_ID;
	d2 = (UINT32) priority;
	/* Setup trap function */
	d0 = SET_PROCESS_PRIORITY;
	asm( "trap #0" );
	success = d0;

	/* Restore registers  */
	asm(" move.l (%a7)+, %d2" );
	asm(" move.l (%a7)+, %d1" );
	asm(" move.l (%a7)+, %d0" );

	return success;
}

int get_process_priority(int process_ID)
{
	int ret;
	register UINT32 d0 asm ("%d0");
	register UINT32 d1 asm ("%d1");

	/* Store registers */
	asm( "move.l %d0, -(%a7)" );
	asm( "move.l %d1, -(%a7)" );

	d1 = (UINT32) process_ID;
	/* Setup trap function */
	d0 = GET_PROCESS_PRIORITY;
	asm( "trap #0" );
	ret = d0;
	/* Restore registers  */
	asm(" move.l (%a7)+, %d1" );
	asm(" move.l (%a7)+, %d0" );

	return ret;
}

UINT32 get_tick_count()
{
	int ret;
	register UINT32 d0 asm ("%d0");
	/* Store registers */
	asm( "move.l %d0, -(%a7)" );
	d0 = GET_TICK_COUNT;
	asm( "trap #0" );
	ret = d0;
	/* Restore registers  */
	asm(" move.l (%a7)+, %d0" );
	return ret;
}


/*
 * kernel.c
 *
 *  Created on: 2010-05-28
 *      Author: mzakharo
 */

/* function returns the address of a stack, belonging to the oNext process
 *
 */
#include "kernel.h"
#include "config.h"
#include "dbug.h"
#include "api_entry.h"
#include "pcb.h"
#include "process.h"
#include "memory.h"
#include "kernel_globals.h"
#include "rtx_api.h"

#undef printf
#define printf(format, ...){rtx_dbug_outs((CHAR *) "rtx_kern: ");\
							printf(format, ## __VA_ARGS__); };

static VOID process_switch(pcb * nextProc)
{
	register UINT32 d0 asm ("%d0");
	register UINT32 d1 asm ("%d1");

	/* Store registers */
	asm( "move.l %d0, -(%a7)" );
	asm( "move.l %d1, -(%a7)" );

	/* Load oNext proc into d1 */
	d1 = (UINT32) nextProc;

	/* Setup trap function */
	d0 = PROC_SWITCH_F;
	asm( "trap #0" );
	/* Restore registers  */
	asm(" move.l (%a7)+, %d1" );
	asm(" move.l (%a7)+, %d0" );

}
VOID kernel_proc_switch(pcb * nextProc)
{
	UINT32 nextprocSP;
	if (currProc != NULL)
		currProc->stack_pointer = API_ENTRY_STACK;
	//switch to a new stack
	nextprocSP = nextProc->stack_pointer;
	//update context pointer
	currProc = nextProc;

    asm("move.l %0, %%sp" : : "m" (nextprocSP));
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
	asm("move.l (%a7)+, %d0");

	asm("rte");
}
void scheduler()
{
	int curr_proc_state = (currProc != NULL) ? currProc->state : -1;

	if (currProc != NULL && curr_proc_state != READY)
	{
		if (currProc->state == SWITCH)
			currProc->state = READY;

		//change state
		if (currProc->state == BLK_ISR)
		{
			currProc->state = READY;
			if (currProc->priority == NULL_P)
				push(&state_q, currProc);
			else
				blkOnIsrProc = currProc;
		}
		else
			push(&state_q, currProc);
	}
#ifdef FOO
	if (currProc != NULL)
	{
		printf("rtx: scheduling from %d", currProc->pid);
	}
#endif

	int memAvalable = (rtx_mem_list_head == NULL) ? 0 : 1;
	int i;
	int max_priority = (curr_proc_state == READY) ? currProc->priority + 1
			: NUMBER_OF_PRIORITIES;

	pcb * next_pcb = NULL;
	for (i = 0; i < max_priority; i++)
	{
		next_pcb = state_q.bins[i].oTail;
		while (next_pcb != NULL)
		{
			if ((memAvalable && next_pcb->state == BLK_MEM) || (next_pcb->state
					== BLK_MSG && next_pcb->mailBox.oTail!= NULL)
					|| (next_pcb->state == READY))
			{
				i = max_priority; //break out of the for loop
				break;
			}
			next_pcb = next_pcb->oPrev;
		}
	}
	//we can only be null if currProc is READY
	if (next_pcb == NULL)
		next_pcb = currProc;
	//we need to switch to the blocked on ISR
	if (blkOnIsrProc != NULL && next_pcb->priority != I_PRIORITY)
	{
		if (blkOnIsrProc->priority <= next_pcb->priority)
			next_pcb = blkOnIsrProc;
		else
			push(&state_q, blkOnIsrProc);
		blkOnIsrProc = NULL;
	}
	pop_pid(&state_q, next_pcb->pid);

	if (next_pcb != currProc)
	{
		if (curr_proc_state == READY)
			push(&state_q, currProc);
		//printf("oNext %d, priority %d", next_pcb->pid, next_pcb->priority);

		next_pcb->state = READY;
		///printf("rtx: scheduling to %d", next_pcb->pid);
		process_switch(next_pcb);
	}
	//printf("rtx: continuing with %d", currProc->pid);

}
/****************************************************
 * used to initialize memory of fixed size to fullfill project
 * requirements
 */
static void init_static_memory()
{
	int i;
	memory_block * memPtr;
	void * req_block;
	rtx_mem_list_head = NULL;
	req_block = kernel_malloc(MEMORY_BLOCK_SIZE);
	if (req_block == NULL)
	{
		printf("out of memory");
		return;
	}
	rtx_mem_list_head
			= container_of(req_block,struct memory_block, user_memory_placeholder);
	rtx_mem_list_head->next = NULL;
	for (i = 0; i < NUMBER_OF_MEMORY_BLOCKS - 1; i++)
	{
		req_block = kernel_malloc(MEMORY_BLOCK_SIZE);
		if (req_block == NULL)
		{
			printf("out of memory");
			return;
		}
		memPtr
				= container_of(req_block,struct memory_block, user_memory_placeholder);
		memPtr->next = rtx_mem_list_head;
		rtx_mem_list_head = memPtr;
	}
	printf("%d static memory blocks initialised", count_memory_blocks(
			rtx_mem_list_head));
}

/**************************************************************************
 The primitive returns a pointer to a memory block
 to the calling process. If no memory block is available, the calling process is
 blocked until a memory block becomes available.
 **************************************************************************/
void * kernel_request_memory_block()
{
	void * request;
	if (rtx_mem_list_head == NULL)
	{
		//never block I_PROCESSES
		if (currProc->procType == SYS_PROC)
			return NULL;
		//put current process at the end of the line
		currProc->state = BLK_MEM;
		scheduler();
		if (rtx_mem_list_head == NULL)
		{
			printf("kernel panic - expected available memory");
			return NULL;
		}

	}
	request = &rtx_mem_list_head->user_memory_placeholder;
	rtx_mem_list_head = rtx_mem_list_head->next;
	return request;
}

/**************************************************************************
 This primitive returns the memory
 block to the RTX. If there are processes waiting for a block, the block is given
 to the highest priority process, which is then unblocked. The caller of this
 primitive never blocks, but could be preempted. If several processes are waiting
 for a memory block and a block becomes available, the highest priority waiting
 process will get it. Thus, it may affect the currently
 executing process.
 **************************************************************************/
int kernel_release_memory_block(void * MemoryBlock)
{
	memory_block * oMem;

	oMem
			= container_of(MemoryBlock, struct memory_block, user_memory_placeholder);
	if (rtx_mem_list_head == NULL)
	{
		rtx_mem_list_head = oMem;
		oMem->next = NULL;
	}
	else
	{
		oMem->next = rtx_mem_list_head;
		rtx_mem_list_head = oMem;
	}
	scheduler();
	return 0;
}

/*enables hardware interrupt masks*/
void init_vbr()
{
	/*
	 * Move the VBR into real memory
	 *
	 * DG: actually, it'll already be here.
	 */
	asm( "move.l %a0, -(%a7)" );
	asm( "move.l #0x10000000, %a0 " );
	asm( "movec.l %a0, %vbr" );
	asm( "move.l (%a7)+, %a0" );

}
VOID kernel_init()
{
	void * start_of_heap = &_end;
	printf("RTX Kernel: initialising...");
	init_memory(start_of_heap, (void *) 0x10200000);
	init_static_memory();
	//initialize the states
	init_queue(&state_q);
	currProc = NULL;
	blkOnIsrProc = NULL;
	//initialize the timer blocked envelopes
	init_rtx_queue((Queue *)&timerMailbox);
	//create PCB structures and stacks for all processes
	init_processes();
	//register the software interrupt for rtx primitives API's
	exceptionHandler(32, api_entry);
	printf("RTX Kernel: initialisation complete, free memory: %d KB",
			count_memory_block_sizes(mem_list_head) / 1024);

}

static pcb * pidToPCB(int process_ID)
{

	pcb* pcbRet = NULL;
	pcbRet = find(&state_q, process_ID);
	if (pcbRet == NULL && currProc->pid == process_ID)
		pcbRet = currProc;
	if (pcbRet == NULL && blkOnIsrProc != NULL &&
            blkOnIsrProc->pid == process_ID)
		pcbRet = blkOnIsrProc;
	return pcbRet;
}
int kernel_delayed_send(int process_ID, void * MessageEnvelope, int delay)
{
	//To account for the fact that the timer increments BEFORE checking messages
	//For example: if there is a message with delay 1,
	//it will get sent on the closest following tick, which is wrong.
	// It should actually be sent on the tick after.
	//The line below "rounds" up to the system time.
	delay++;

	//search for process_id (if it exists)
	pcb* pcbTo = pidToPCB(process_ID);
	if (pcbTo == NULL)
		return RTX_ERROR;

	messageEnvelope * msgEnv = (messageEnvelope *) MessageEnvelope;
	push_rtx_queue((Queue *)&timerMailbox, MessageEnvelope);
	msgEnv->send_time = delay + timer0Count;
	msgEnv->sender_process_id = currProc->pid;
	msgEnv->destination_process_id = process_ID;

	return RTX_SUCCESS;
}

UINT32 kernel_get_timer0_tick()
{
	return timer0Count;
}

/* The last parameter controls whether the message sender_process_id is filled out from
 * currProc or whether it's just left alone
 */
int kernel_send_message(int process_ID, void * MessageEnvelope, int kernel_flag)
{
	messageEnvelope * msgEnv = (messageEnvelope *) MessageEnvelope;

	pcb* pcbTo = pidToPCB(process_ID);
	if (pcbTo == NULL)
		return RTX_ERROR;

	if (kernel_flag == 0) //Otherwise, let it be, it must have been filled out already
	{
		msgEnv->sender_process_id = currProc->pid;
	}
	msgEnv->destination_process_id = process_ID;
	//Implementation: one queue (as a linked list) per process

	//fetch the proper PCB
	//printf("rtx: %d sending message to %d", currProc->pid, process_ID);
	push_rtx_queue(&pcbTo->mailBox, MessageEnvelope);
	scheduler();
	return RTX_SUCCESS;
}

void * kernel_receive_message(int * sender_ID)
{
	messageEnvelope * toRet;
	toRet = (messageEnvelope *) pop_rtx_queue(&currProc->mailBox);
	if (toRet == NULL)
	{
		currProc->state = BLK_MSG;
		scheduler();
		toRet = (messageEnvelope *) pop_rtx_queue(&currProc->mailBox);
		if (toRet == NULL)
		{
			printf("kernel panic - expected to receive a message");
			kernel_send_hotkey('#');
			while (1)
				;
			return NULL;
		}
	}
	//printf("rtx: %d received message from %d", currProc->pid, toRet->sender_process_id);
	toRet->oNext= NULL;
	toRet->oPrev = NULL;
	if (sender_ID != NULL)
		*sender_ID = toRet->sender_process_id;
	return toRet;
}

int kernel_release_processor()
{
	currProc->state = SWITCH;
	scheduler();
	return 0;
}
void kernel_send_hotkey(BYTE hotkey)
{
	switch (hotkey)
	{
	case '~':
		printf("---------------------");
		printf("Hot Key: ~ Printing Ready Queue:");
		print_pqueue(&state_q, READY);
		printf("---------------------");
		break;
	case '!':
		printf("---------------------");
		printf("Hot Key: ! Printing Blocked on Memory Queue:");
		print_pqueue(&state_q, BLK_MEM);
		printf("---------------------");
		break;
	case '@':
		printf("---------------------");
		printf("Hot Key: @ Printing Blocked on Message Queue:");
		print_pqueue(&state_q, BLK_MSG);
		printf("---------------------");
		break;
	case '#':
		printf("---------------------");
		printf("Hot Key: # Printing Blocked on Message Queue:");
		print_pqueue(&state_q, BLK_MSG);
		printf("Hot Key: ! Printing Blocked on Memory Queue:");
		print_pqueue(&state_q, BLK_MEM);
		printf("Hot Key: ~ Printing Ready Queue:");
		print_pqueue(&state_q, READY);
		printf("Hot Key: ~ Printing currProc");
		printf("PCB ID is %d (%s), priority %d", currProc->pid, pid_to_string(
				currProc->pid), currProc->priority);
		printf("Hot Key: ~ Printing blked On IRQ:");
		if (blkOnIsrProc != NULL)
		{
			printf("PCB ID is %d (%s), priority %d", blkOnIsrProc->pid,
					pid_to_string(blkOnIsrProc->pid), blkOnIsrProc->priority);
		}
		printf("---------------------");
	};

}
int kernel_get_process_priority(int process_ID)
{

	pcb* oPcb = pidToPCB(process_ID);
	if (oPcb == NULL)
		return RTX_ERROR;
	return oPcb->priority;
}

int kernel_set_process_priority(int process_ID, int new_priority)
{
	signed int old_priority;
	pcb* oPcb = pidToPCB(process_ID);
	if (oPcb == NULL || oPcb->priority == NULL_P || new_priority == NULL_P)
		return RTX_ERROR;
	old_priority = oPcb->priority;
	oPcb->priority = (signed int) new_priority;
	//case 1 - we have lowered ourselves

	if (oPcb != currProc && old_priority != new_priority)
	{
		//if you are in the q
		if (find(&state_q, process_ID) != NULL)
			push(&state_q, pop_pid(&state_q, process_ID));
	}
	scheduler();
	return RTX_SUCCESS;
}

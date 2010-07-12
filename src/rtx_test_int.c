/*--------------------------------------------------------------------------
 *                      RTX Test Suite 
 *--------------------------------------------------------------------------
 */
/**
 * @file:   rtx_test_dummy.c   
 * @author: Thomas Reidemeister
 * @author: Irene Huang
 * @date:   2010.02.11
 * @brief:  rtx test suite 
 */

#include "rtx_test.h"
#include "dbug.h"
#include "random_gen.h"
#include "rtx_api.h"
int g_dummy;
void infinite_loop();
#define error(format, ...) {rtx_dbug_outs((CHAR *) "ERROR: "); \
							printf(format, ##__VA_ARGS__); \
							infinite_loop();}
/* third party dummy test process 1 */
enum
{
	S0, S1, S2, S3, S4, S5, S6, S7, S8, S9,
};

int g_current_test;
enum TEST_CASES
{
	KERNEL_MALLOC, REG_CMD, REG_DUP_CMD, TEST_COMPLETE,
};
//mutex send and receive
void receive();
void send(int dest_PID);
void check_state(int state);
void set_state(int state);
void kernel_malloc_test();
void reg_cmd_test();

int g_state_machine;
void test1()
{
	rtx_dbug_outs((CHAR *) "rtx_test: test1\r\n");
	g_current_test = KERNEL_MALLOC;
	set_state(S0);
	kernel_malloc_test();
	while (1)
	{
		g_test_fixture.release_processor();
	}
}

/* third party dummy test process 2 */
void test2()
{
	rtx_dbug_outs((CHAR *) "rtx_test: test2\r\n");
	int nRecievePID;
	messageEnvelope *oMsgPtr;

	g_current_test = REG_CMD;
	reg_cmd_test();

	while (1)
	{
		switch (g_current_test)
		{
		case (REG_CMD):
			oMsgPtr = (messageEnvelope*) g_test_fixture.receive_message(
					&nRecievePID);
			printf("Test 2 received: '%s'", oMsgPtr->data);
			g_current_test = TEST_COMPLETE;
			send(3);
			break;
		case (TEST_COMPLETE):
			g_test_fixture.release_processor();
		default:
			g_test_fixture.release_processor();
		}
	}
}
/* third party dummy test process 3 */
void test3()
{
	rtx_dbug_outs((CHAR *) "rtx_test: test3\r\n");
	int nRecievePID;
	messageEnvelope *oMsgPtr;
	receive();
	g_current_test = REG_DUP_CMD;
	reg_cmd_test();
	while (1)
	{
		switch (g_current_test)
		{
		case (REG_DUP_CMD):
			oMsgPtr = (messageEnvelope*) g_test_fixture.receive_message(
					&nRecievePID);
			printf("Test 3 received: '%s'", oMsgPtr->data);
			g_current_test = TEST_COMPLETE;
			break;
		case (TEST_COMPLETE):
			g_test_fixture.release_processor();
		default:
			g_test_fixture.release_processor();
		}
	}
}
/* third party dummy test process 4 */
void test4()
{
	rtx_dbug_outs((CHAR *) "rtx_test: test4\r\n");
	while (1)
	{
		receive();
		/* execute a rtx primitive to test */
		g_test_fixture.release_processor();
	}
}
/* third party dummy test process 5 */
void test5()
{
	rtx_dbug_outs((CHAR *) "rtx_test: test5\r\n");
	while (1)
	{
		receive();
		/* execute a rtx primitive to test */
		g_test_fixture.release_processor();
	}
}
/* third party dummy test process 6 */
void test6()
{
	rtx_dbug_outs((CHAR *) "rtx_test: test6\r\n");
	while (1)
	{
		receive();
		/* execute a rtx primitive to test */
		g_test_fixture.release_processor();
	}
}

/* register the third party test processes with RTX */
void __attribute__ ((section ("__REGISTER_TEST_PROCS__"))) register_test_proc()
{
	int i;

	rtx_dbug_outs((CHAR *) "rtx_test: register_test_proc()\r\n");

	for (i = 0; i < NUM_TEST_PROCS; i++)
	{
		g_test_proc[i].pid = i + 1;
		g_test_proc[i].priority = LOW;
		g_test_proc[i].sz_stack = 4096;
	}
	g_test_proc[0].entry = test1;
	g_test_proc[1].entry = test2;
	g_test_proc[2].entry = test3;
	g_test_proc[3].entry = test4;
	g_test_proc[4].entry = test5;
	g_test_proc[5].entry = test6;
}

/**
 * Main entry point for this program.
 * never get invoked
 */
int main(void)
{
	rtx_dbug_outs((CHAR *) "rtx_test: started\r\n");
	return 0;
}

void infinite_loop()
{
	while (1)
		;
}

void check_state(int desired_state)
{
	if (g_state_machine != desired_state)
	{
		error("current state is %d, but should be %d",
				g_state_machine, desired_state);
	}
}
void set_state(int desired_state)
{
	g_state_machine = desired_state;
}

void receive()
{
	msgEnv_t * msg = g_test_fixture.receive_message(&g_dummy);
	if (msg == NULL)
	{
		error("received a null message - Mutex fail");
	}
	g_test_fixture.release_memory_block(msg);
}

void send(int dest_PID)
{
	msgEnv_t * msg;
	msg = (msgEnv_t*) g_test_fixture.request_memory_block();
	g_test_fixture.send_message(dest_PID, msg);
}
/*
void send_cust_msg( char chr, int pid )
{
	messageEnvelope *oMessage = NULL;
	oMessage = (messageEnvelope*) g_test_fixture.request_memory_block();
	if( oMessage == NULL )
		error("request memory block failed");

	int i;
	for( i = 0; i < MESSAGE_DATA_SIZE; i++ )
	{
		oMessage->data[i] = chr;
	}
	if( g_test_fixture.send_message( pid, oMessage ) == RTX_ERROR )
		error("sending message failed");

}
void send_cust_msg_to_invalid( )
{
	messageEnvelope *oMessage = NULL;
	oMessage = (messageEnvelope*) g_test_fixture.request_memory_block();
	if( oMessage == NULL )
		error("request memory block failed");

	int i;
	for( i = 0; i < MESSAGE_DATA_SIZE; i++ )
	{
		oMessage->data[i] = 'z';
	}
	if( g_test_fixture.send_message( 9999, oMessage ) != RTX_ERROR )
	{
		error("sending message to invalid process succeeded");
	}
}
void send_cust_delay_msg_to_invalid( )
{
	messageEnvelope *oMessage = NULL;

	oMessage = (messageEnvelope*) g_test_fixture.request_memory_block();
	if( oMessage == NULL )
		error("request memory block failed");

	int i;
	for( i = 0; i < MESSAGE_DATA_SIZE; i++ )
	{
		oMessage->data[i] = 'z';
	}

	if( g_test_fixture.delayed_send( 9999, oMessage, 5 ) != RTX_ERROR )
	{
		error("sending delayed message to invalid process succeeded");
	}
}
void send_cust_msg_delay( char chr, int pid, int delay )
{
	messageEnvelope *oMessage = NULL;

	oMessage = (messageEnvelope*) g_test_fixture.request_memory_block();
	int i = 0;
	for( i = 0; i < MESSAGE_DATA_SIZE; i++ )
	{
		oMessage->data[i] = chr;
	}
	if( g_test_fixture.delayed_send( pid, oMessage, delay ) == RTX_ERROR )
	{
		printf("ERROR");
	}
}
void receive_cust_msg( char chrExp, int procExp )
{
	int i = 0;
	messageEnvelope *oMessage = NULL;
	oMessage = g_test_fixture.receive_message( &i );
	if( NULL == oMessage )
	{
		error("received NULL pointer as message");
	}
	int j;
	if( oMessage->sender_process_id != procExp )
	{
		error("expected different sender process from message");
	}
	for( j = 1; j < MESSAGE_DATA_SIZE; j++ )
	{
		if( oMessage->data[0] != chrExp )
		{
			error("expected character %c in message", chrExp );
			break;
		}
	}
	//printf("received message with char %c from proc %d", oMessage->data[0], i );
	g_test_fixture.release_memory_block( oMessage );
}

void message_passing_test()
{

}*/

void get_set_priority_tests();
#define nMaxMemHistory 100
void kernel_malloc_test()
{
	//Determine initial free memory.
	long int nFreeMem = g_test_fixture.count_mem(1);
	long int nRqstSize = nFreeMem / 2;
	//Create an array of pointers.
	void *oMemHistory[nMaxMemHistory];
	int nCount = 0;

	//Allocate and then free one big block of memory and ensure that we cannot allocate more.
	oMemHistory[0] = g_test_fixture.malloc(nRqstSize);
	nCount++;
	if (oMemHistory[0] != NULL)
	{
		oMemHistory[1] = g_test_fixture.malloc(nRqstSize + 10);
		if (oMemHistory[1] == NULL)
		{
			//Check and ensure that there is still 1 free memory block.
			if (g_test_fixture.count_mem(0) == 1)
			{
				//Free the block.
				g_test_fixture.free(oMemHistory[0]);
				if (g_test_fixture.count_mem(1) == nFreeMem
						&& g_test_fixture.count_mem(0) == 1)
				{
					//Set state to allow for execution of oNext part of test.
					set_state(S1);
					nCount = 0;
				}
				else
				{
					error("Kernel Malloc 1 Test Failed: Memory not at initial state after free.");
				}
			}
			else
			{
				error("Kernel Malloc 1 Test Failed: More then 1 free memory block, expected 1.");
			}
		}
		else
		{
			error("Kernel Malloc 1 Test Failed: Allocated memory exceeding memory space.");
		}
	}
	else
	{
		error("Kernel Malloc Test 1 Failed: Did not allocate a block of 50%% of memory.");
	}
	//This test state allocates 5 memory blocks: 50%, 25%, 12.5%, 6.25%, and 3.125% of memory and
	//performs free operations to test the defragmentation functionality of memory.
	check_state(S1);

	oMemHistory[0] = g_test_fixture.malloc(nFreeMem / 2);
	oMemHistory[1] = g_test_fixture.malloc(nFreeMem / 4);
	oMemHistory[2] = g_test_fixture.malloc(nFreeMem / 8);
	oMemHistory[3] = g_test_fixture.malloc(nFreeMem / 16);
	oMemHistory[4] = g_test_fixture.malloc(nFreeMem / 32);
	nCount = 5;

	if (oMemHistory[0] != NULL && oMemHistory[1] != NULL && oMemHistory[2]
			!= NULL && oMemHistory[3] != NULL && oMemHistory[4] != NULL)
	{
		if (g_test_fixture.count_mem(0) == 1 && g_test_fixture.count_mem(1)
				<= (nFreeMem / 32))
		{
			//Free the 25% block and ensure that there are 2 free blocks.
			g_test_fixture.free(oMemHistory[1]);
			if (g_test_fixture.count_mem(0) == 2)
			{
				//Free the 3% block and ensure that lower fragmentation works (2 free blocks).
				g_test_fixture.free(oMemHistory[4]);
				if (g_test_fixture.count_mem(0) == 2)
				{
					//Free the 12.5% block and ensure upper fragmentation works(2 free blocks).
					g_test_fixture.free(oMemHistory[2]);
					if (g_test_fixture.count_mem(0) == 2)
					{
						//Free the 6.25% block and ensure upper/lower fragmentation works(1 free block).
						g_test_fixture.free(oMemHistory[3]);
						if (g_test_fixture.count_mem(0) == 1)
						{
							//Free last memory block and reconcile.
							g_test_fixture.free(oMemHistory[0]);
							if (g_test_fixture.count_mem(1) == nFreeMem
									&& g_test_fixture.count_mem(0) == 1)
							{
								//Set state to allow for execution of oNext part of test.
								set_state(S2);
								nCount = 0;
							}
							else
							{
								error("Kernel Malloc 2 Test Failed: Memory reconciliation failed.");
							}

						}
						else
						{
							error("Kernel Malloc 2 Test Failed: Upper/Lower fragmentation failed.");
						}
					}
				}
				else
				{
					error("Kernel Malloc 2 Test Failed: Upper fragmentation failed.");
				}
			}
			else
			{
				error("Kernel Malloc 2 Test Failed: Lower fragmentation failed.");
			}
		}
		else
		{
			error("Kernel Malloc 2 Test Failed: Lower fragmentation failed.");
		}
	}
	else
	{
		error("Kernel Malloc 2 Test Failed: Available memory not what expected.");
	}

	//This test uses a random number generator to stress test memory.
	check_state(S2);

	long int nRandom = 0;
	long int nRemainingMem = nFreeMem;
	nCount = 0;

	init_twister();
	sgenrand(0, nFreeMem);
	nRandom = genrand(nCount);
	if (nRandom < 0)
	{
		nRandom = nRandom * -1;
	}
	//Normalize random number.
	nRandom = (nRandom & nRemainingMem) >> 1;
	nRemainingMem = nRemainingMem - nRandom;
	oMemHistory[nCount] = g_test_fixture.malloc(nRandom);
	while (oMemHistory[nCount] != NULL && nCount < 100)
	{
		nCount++;
		nRandom = genrand(nRandom);
		if (nRandom < 0)
		{
			nRandom = nRandom * -1;
		}
		//Normalize random number.

		nRandom = (nRandom & nRemainingMem) >> 1;

		nRemainingMem = nRemainingMem - nRandom;
		oMemHistory[nCount] = g_test_fixture.malloc(nRandom);
	}
	//Begin freeing in order of allocation.
	int nFreeCount = 0;
	while (nFreeCount < nCount)
	{
		g_test_fixture.free(oMemHistory[nFreeCount]);
		nFreeCount++;
	}
	//Reconcile Memory
	if (g_test_fixture.count_mem(1) == nFreeMem && g_test_fixture.count_mem(0)
			== 1)
	{
		//Set state to allow for execution of oNext part of test.
		set_state(S3);
		nCount = 0;
	}
	else
	{
		error("Kernel Malloc 3 Test Failed: Memory reconciliation failed.");
	}

	check_state(S3);
	printf("Kernel Malloc Internal Tests Pass.");

	int nClean = 0;

	while (nClean < nCount)
	{
		g_test_fixture.free(oMemHistory[nClean]);
		nClean++;
	}
	g_current_test = TEST_COMPLETE;
}

void reg_cmd_test()
{
	//Send a message to register a command.
	messageEnvelope *oMsgPtr =
			(messageEnvelope*) g_test_fixture.request_memory_block();
	oMsgPtr->message_type = 3;
	oMsgPtr->data[0] = 'Q';
	//Send to KCD to register.
	g_test_fixture.send_message(SYS_KEYBOARD_DECODER_PID, oMsgPtr);
	oMsgPtr = (messageEnvelope*) g_test_fixture.request_memory_block();
	oMsgPtr->message_type = 3;
	oMsgPtr->data[0] = 'A';
	//Send to KCD to register.
	g_test_fixture.send_message(SYS_KEYBOARD_DECODER_PID, oMsgPtr);
}


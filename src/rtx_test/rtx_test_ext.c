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
#include "rtx_test_common.h"
#include "fixture.h"

//globals
int g_dummy;
int g_completed_tests;
int g_failed_tests;

void test1()
{
	printf("START");
	printf("total %d tests", NUMBER_OF_TESTS);
	basic_priority_test1();
	g_completed_tests++;
	req_rel_mem_blk_test1();
	g_completed_tests++;
	basic_message_test1();
	g_completed_tests++;
	basic_release_processor_test1();
	g_completed_tests++;
	/*printf("basic tests complete, starting advanced procedures");
	 advanced_memory_schedueling_test1();
	 g_completed_tests++;
	 */
	printf("stability tests started for 5 seconds (repeating message and memory tests)");
	int i;
	for (i = 0; i < 5; i++)
	{
		req_rel_mem_blk_test1();
		basic_message_test1();
		/* pause */
		msgEnv_t * tmp = g_test_fixture.request_memory_block();
		g_test_fixture.delayed_send(1, tmp, 1000);
		g_test_fixture.receive_message(&g_dummy);
		g_test_fixture.release_memory_block(tmp);
	}
	g_completed_tests++;
	printf("Testing complete!");
	print_results();
	receive(); //block the last test process
	while (1)
	{
		g_test_fixture.release_processor();
	}

}

/* third party dummy test process 2 */
void test2()
{
	printf("test2 proc started");
	while (1)
	{
		receive();
		switch (g_current_test)
		{
		case (GET_SET_PRIORITY):
			basic_priority_test2();
			break;
		case (MEMORY_ORDER):
			advanced_memory_schedueling_test2();
			break;

		case (REC_REL_MEM_BLK):
			req_rel_mem_blk_test2();
			break;

		case (MESSAGES):
			basic_message_test2();
			break;
		case (RELEASE_PROCESSOR):
			basic_release_processor_test2();
			break;
		default:
			g_test_fixture.release_processor();
		};
	}
}
/* third party dummy test process 3 */
void test3()
{

	printf("test3 proc started");
	while (1)
	{
		receive();
		switch (g_current_test)
		{
		case (GET_SET_PRIORITY):
			basic_priority_test3();
			break;
		case (MEMORY_ORDER):
			advanced_memory_schedueling_test3();
			break;
		case (RELEASE_PROCESSOR):
			basic_release_processor_test3();
			break;
		default:
			g_test_fixture.release_processor();
		};
	}
}
/* third party dummy test process 4 */
void test4()
{
	printf("test4 proc started");
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
	printf("test5 proc started");
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
	receive();
	/*
	 msgEnv_t *msg;
	 printf("test6 proc started");
	 g_test_fixture.set_process_priority(6,HIGH);

	 */
	while (1)
	{
		/*
		 msg = g_test_fixture.request_memory_block();
		 g_test_fixture.delayed_send(6, msg, 10);
		 msg = g_test_fixture.receive_message(&g_dummy);
		 g_test_fixture.release_memory_block(msg);
		 */
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
		g_test_proc[i].priority = LOWEST;
		g_test_proc[i].sz_stack = 4096;
	}
	g_test_proc[0].entry = test1;
	g_test_proc[1].entry = test2;
	g_test_proc[2].entry = test3;
	g_test_proc[3].entry = test4;
	g_test_proc[4].entry = test5;
	g_test_proc[5].entry = test6;

	//testing variables set
	g_current_test = TEST_COMPLETE;
	g_completed_tests = 0;
	g_failed_tests = 0;
}

/**
 * Main entry point for this program.
 * never get invoked
 */
int main(void)
{
	return 0;
}
void print_results()
{
	printf("%d/%d tests OK", g_completed_tests, NUMBER_OF_TESTS);
	printf("%d/%d tests FAIL",g_failed_tests, NUMBER_OF_TESTS);
	printf("%d/%d tests NOT RUN",NUMBER_OF_TESTS - g_failed_tests - g_completed_tests, NUMBER_OF_TESTS);
	printf("END");
}

void infinite_loop()
{
	g_failed_tests++;
	print_results();
	while (1)
		;
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
	msgEnv_t * msg = g_test_fixture.request_memory_block();
	if (msg == NULL)
	{
		error("received a null memory address- Mutex fail");
	}
	g_test_fixture.send_message(dest_PID, msg);
}

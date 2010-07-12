/*
 * advanced_memory_schedueling.c
 *
 *  Created on: Jul 9, 2010
 *      Author: mzakharo
 */


#include "rtx_inc.h"
#include "rtx_test.h"
#include "rtx_test_common.h"


#define MAX_MEMORY_BLOCKS 10000
void *g_memory[MAX_MEMORY_BLOCKS];
int g_memory_index;

void advanced_memory_schedueling_test1()
{
	int i;
	int dummy;

	g_current_test = MEMORY_ORDER;
	printf("advanced scheduling test started");
	set_state(S0);
	int test1_orig_p = g_test_fixture.get_process_priority(1);
	int test2_orig_p = g_test_fixture.get_process_priority(2);
	int test3_orig_p = g_test_fixture.get_process_priority(3);
	g_test_fixture.set_process_priority(1, MEDIUM);
	g_test_fixture.set_process_priority(2, HIGH);
	g_test_fixture.set_process_priority(3, HIGH);
	g_memory_index = 0;
	check_state(S0, "test1 should be the highest priority");
	g_memory[g_memory_index++] = g_test_fixture.request_memory_block();
	set_state(S1);
	send(2);
	g_test_fixture.delayed_send(1, g_memory[0],20);
	g_test_fixture.receive_message(&dummy);

	check_state(S2,
			"test2 should have requested all avalable memory blocks and pre-empted");

	set_state(S3);
	send(3);
	check_state(S4, "test3 should have pre-empted on request memory block ");
	printf("test is using %d mem blks", g_memory_index);
	g_test_fixture.set_process_priority(1, HIGH);
	g_test_fixture.set_process_priority(2, MEDIUM);
	g_test_fixture.set_process_priority(3, MEDIUM);
	//release and request memory block to block and unblock a process
	g_test_fixture.release_memory_block(g_memory[--g_memory_index]);
	check_state(S4,
			"releasing memory block should not pre-empt, I have the highest  priority");
	g_memory[g_memory_index++] = g_test_fixture.request_memory_block();
	check_state(S4, "OS should give the memory block back to the highest");
	//cycle 2 from ready back to blocked (will break some implementations)
	g_test_fixture.set_process_priority(1, LOW);
	check_state(S4, "test1 is the only one unblocked - should have executed");
	set_state(S5);
	g_test_fixture.release_memory_block(g_memory[--g_memory_index]);
	printf("xoxo");
	receive();
	check_state(S6, "test2 should have received a memory block ");
	printf("memory order test with one process PASS");
	g_test_fixture.set_process_priority(1, HIGH);
	//test for order with 2 processes blocked on memory
	//blocke on memory at LOW   head->2->3->NULL
	//process 3 should run, then process 2
	g_test_fixture.release_memory_block(g_memory[--g_memory_index]);
	check_state(S6, "test1 should not pre-empt ");
	g_test_fixture.release_memory_block(g_memory[--g_memory_index]);
	check_state(S6, "test1 should not pre-empt ");
	g_memory[g_memory_index++] = g_test_fixture.request_memory_block();
	check_state(S6, "test1 should not pre-empt ");
	g_memory[g_memory_index++] = g_test_fixture.request_memory_block();
	check_state(S6, "test1 should not pre-empt ");
	//cycle processes from queues  - should break some implememnations
	g_test_fixture.set_process_priority(1, LOW);
	check_state(S6,
			"test1 should be the first to run after requesting all memory blocks");
	g_test_fixture.set_process_priority(1, HIGH);
	//lets see if the above is true:
	set_state(S7);
	g_test_fixture.release_memory_block(g_memory[--g_memory_index]);
	check_state(S7, "test1 should not pre-empt ");
	g_test_fixture.release_memory_block(g_memory[--g_memory_index]);
	check_state(S7, "test1 should not pre-empt ");
	g_test_fixture.set_process_priority(1, LOW);
	check_state(S9,
			"test2 and test3 should have received a memory block in proper order");
	g_test_fixture.set_process_priority(1, HIGH);
	printf("memory scheduling test PASSED");
	//cleanup
	for (i = 0; i < g_memory_index; i++)
	{
		g_test_fixture.release_memory_block(g_memory[i]);
	}
	g_test_fixture.set_process_priority(2, test2_orig_p);
	g_test_fixture.set_process_priority(3, test3_orig_p);
	g_test_fixture.set_process_priority(1, test1_orig_p);
	g_current_test = TEST_COMPLETE;
}

void advanced_memory_schedueling_test2()
{
	int i;
	check_state(S1, "test2 is not yet ready to run ");
	set_state(S2);
	for (i = 0; i < MAX_MEMORY_BLOCKS; i++)
	{
		g_memory[g_memory_index] = g_test_fixture.request_memory_block();
		g_memory_index++;
		if (get_state() > S2)
			break;

	}
	if (i == MAX_MEMORY_BLOCKS)
	{
		error("the OS has declared too many memory blocks, unable to proceed");
	}
	check_state(S5, "test2 should still be blocked on memory");
	send(1);
	set_state(S6);
	g_memory[g_memory_index] = g_test_fixture.request_memory_block();
	g_memory_index++;
	check_state(S8, "test2 should still be blocked on memory");
	set_state(S9);

}
void advanced_memory_schedueling_test3()
{
	check_state(S3, "test3 execution is not yet allowed");
	set_state(S4);
	g_memory[g_memory_index] = g_test_fixture.request_memory_block();
	g_memory_index++;
	if (get_state() == S4)
	{
		error("should get blocked on memory for test %d", 3);
	}
	check_state(S7, "test3 should still be blocked on memory");
	set_state(S8);

}

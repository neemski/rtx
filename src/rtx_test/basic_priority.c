/*
 * basic_priority.c
 *
 *  Created on: Jul 9, 2010
 *      Author: mzakharo
 */
#include "rtx_inc.h"
#include "rtx_test.h"
#include "rtx_test_common.h"
/* this testcase checks if get and set priority works,
 * as well as pre-emption. If this test fails, the testing suite
 * fails as well
 */
void basic_priority_test1()
{
	g_current_test = GET_SET_PRIORITY;
	set_state(S0);
	//store current state of processes
	int test1_orig_p = g_test_fixture.get_process_priority(1);
	int test2_orig_p = g_test_fixture.get_process_priority(2);
	int test3_orig_p = g_test_fixture.get_process_priority(3);
	int i;
	printf("priority tests started");
	//set my priority
	for (i = 0; i < 4; i++)
	{
		g_test_fixture.set_process_priority(1, i);
		check_state(S0, "test1 is highest - should not pre-empt");
		if (g_test_fixture.get_process_priority(1) != i)
			error("unable to set priority of PID %d to %d", 1, i);
	}
	//set someone else's priority
	for (i = 0; i < 4; i++)
	{
		g_test_fixture.set_process_priority(2, i);
		check_state(S0, "test1 is highest - should not pre-empt");
		if (g_test_fixture.get_process_priority(2) != i)
			error("unable to set priority of PID %d to %d", 2, i);
	}

	//setup testcase priorities
	g_test_fixture.set_process_priority(1, HIGH);
	check_state(S0, "test1 is highest - should not pre-empt");
	g_test_fixture.set_process_priority(2, HIGH);
	check_state(S0, "test1 is highest - should not pre-empt");
	check_state(S0, "test1 is highest - should not pre-empt");
	g_test_fixture.set_process_priority(3, LOW);
	check_state(S0, "test1 is highest - should not pre-empt");

	//unblock test2
	send(2);
	check_state(S0, "test1 is highest - should not pre-empt");
	//setting myself  lower than a currently unblocked process
	//should pre-empt here
	set_state(S1);
	g_test_fixture.set_process_priority(1, MEDIUM);
	check_state(S2, "test2 should have executed");
	//test second condition - ublock someone else and set them higher

	//should not pre-empt - 3 is lower in priority
	send(3);
	check_state(S2, "test1 is highest - shold not pre-empt");
	set_state(S3);
	g_test_fixture.set_process_priority(3, HIGH);
	check_state(S4, "should pre-empt to the newly highest priroty process");
	g_test_fixture.set_process_priority(1, HIGH);

	printf("priority tests PASS!");
	//cleanup
	g_test_fixture.set_process_priority(2, test2_orig_p);
	g_test_fixture.set_process_priority(3, test3_orig_p);
	g_test_fixture.set_process_priority(1, test1_orig_p);
	g_current_test = TEST_COMPLETE;

}

void basic_priority_test2()
{
	check_state(S1, "test2 is not yet ready to run ");
	set_state(S2);

}

void basic_priority_test3()
{
	check_state(S3, "not highest priority process executed");
	set_state(S4);
}

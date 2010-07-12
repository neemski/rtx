/*
 * basic_release_processor.c
 *
 *  Created on: Jul 9, 2010
 *      Author: mzakharo
 */

#include "rtx_inc.h"
#include "rtx_test.h"
#include "rtx_test_common.h"

/*basic release processor tests, with pre-emptive functionality being
 * tested
 */
void basic_release_processor_test1()
{
	printf("started release processor tests");
	g_current_test = RELEASE_PROCESSOR;
	int test1_orig_p = g_test_fixture.get_process_priority(1);
	int test2_orig_p = g_test_fixture.get_process_priority(2);
	int test3_orig_p = g_test_fixture.get_process_priority(3);
	set_state(S0);
	g_test_fixture.set_process_priority(1, HIGH);
	g_test_fixture.set_process_priority(2, MEDIUM);
	g_test_fixture.set_process_priority(3, MEDIUM);
	g_test_fixture.set_process_priority(2, HIGH);
	g_test_fixture.set_process_priority(3, HIGH);
	send(2);
	send(3);
	check_state(S0, "test1 pre-emption should not occur yet");
	set_state(S1);
	g_test_fixture.release_processor();
	check_state(S3, "expected test2 and test3 pocesses of the same priority to execute ");
	g_test_fixture.set_process_priority(2, LOW);
	g_test_fixture.set_process_priority(3, MEDIUM);
	set_state(S4);
	send(2);
	send(3);
	g_test_fixture.release_processor();
	check_state(S4, "excepted not to pre-empt - I test1 is still highest ");
	g_test_fixture.set_process_priority(1, LOWEST);
	check_state(S6, "test2 and test3 should run first " )
	g_test_fixture.set_process_priority(1, HIGH);

	printf("Release processor tests PASS!");

	g_test_fixture.set_process_priority(2, test2_orig_p);
	g_test_fixture.set_process_priority(3, test3_orig_p);
	g_test_fixture.set_process_priority(1, test1_orig_p);

	g_current_test = TEST_COMPLETE;
}
void basic_release_processor_test2()
{
	check_state(S1, "entered test2 proc prematurely")
	set_state(S2);
	g_test_fixture.release_processor();
	if (get_state() == S2)
	{
		error("pre-emption should occur");
	}
	receive();
	check_state(S5, "entered test2 proc prematurely")
	set_state(S6);
}
void basic_release_processor_test3()
{
			check_state(S2, "test3 execution is not yet allowed, should run test1 and test2 first" )
			set_state(S3);
			g_test_fixture.release_processor();
			if (get_state() == S3)
			{
				error("preemption should occur");
			}
			receive();
			check_state(S4, "entered test3 process prematurely")
			set_state(S5);


}

/*
 * basic_mem.c
 *
 *  Created on: 2010-07-09
 *      Author: Michael Bieniek
 */

#include "rtx_inc.h"
#include "rtx_test.h"
#include "rtx_test_common.h"

#define nMaxMemHistory  100
/*checks if the operating system has at least 30 memory blocks available
 *
 */
void req_rel_mem_blk_test1()
{
	g_current_test = REC_REL_MEM_BLK;
	printf("request release memory block tests started");
	int test1_orig_p = g_test_fixture.get_process_priority(1);
	int test2_orig_p = g_test_fixture.get_process_priority(2);
	g_test_fixture.set_process_priority(1, HIGH);
	g_test_fixture.set_process_priority(2, HIGH);
	set_state(S0);
	send(2);
	//Create an array of pointers.
	void *oMemHistory[nMaxMemHistory];
	int nCount = 0;
	int nClean = 0;
	int nMemoryBlocks = 29;
	//State 1: Requesting and releasing memory blocks.
	while (nCount < 6)
	{
		oMemHistory[nCount] = g_test_fixture.request_memory_block();
		if (oMemHistory[nCount] == NULL)
		{
			error("Req/Rel Mem Blk 1 Test Failed: Request memory block returned NULL.");
		}
		else
			nCount++;
	}
	while (nClean < nCount)
	{
		g_test_fixture.release_memory_block(oMemHistory[nClean]);
		nClean++;
	}
	nClean = 0;

	set_state(S1);

	//State 2: Ensure there are atleast 30 memory blocks available as per specs.
	nCount = 0;

	while (nCount < nMemoryBlocks)
	{
		oMemHistory[nCount] = g_test_fixture.request_memory_block();
		if (oMemHistory[nCount] == NULL)
		{
			error("Req/Rel Mem Blk 1 Test Failed: Returned NULL");
		}
		else
			nCount++;
	}

	while (nClean < nCount)
	{
		g_test_fixture.release_memory_block(oMemHistory[nClean]);
		nClean++;
	}
	//cleanup
	set_state(S2);
	g_test_fixture.set_process_priority(1, MEDIUM);
	check_state(S3, "test2 should have executed");
	printf("Request and Release Memory External Tests Pass.");
	g_test_fixture.set_process_priority(1, test1_orig_p);
	g_test_fixture.set_process_priority(2, test2_orig_p);
	g_current_test = TEST_COMPLETE;
}

void req_rel_mem_blk_test2()
{
	check_state(S2,"Request and Release Memory Test Failed: Less than 30 memory blocks." );
	set_state(S3);
}

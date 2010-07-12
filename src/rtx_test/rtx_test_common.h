/*
 * rtx_test_common.h
 *
 *  Created on: Jul 9, 2010
 *      Author: mzakharo
 */

#ifndef RTX_TEST_COMMON_H_
#define RTX_TEST_COMMON_H_
#include "basic_message.h"
#include "basic_priority.h"
#include "basic_release_processor.h"
#include "basic_mem.h"
#include "advanced_memory_schedueling.h"
#include "dbug.h"

int g_current_test;
int g_state_machine;

void infinite_loop();
void print_results();
#define error(format, ...) {printf("error occured in test %d, will not continue...", g_current_test); \
							printf(format, ##__VA_ARGS__); \
							infinite_loop();}
#undef printf
#define printf(format, ...){rtx_dbug_outs((CHAR *) "G07_test: ");\
							printf(format, ## __VA_ARGS__); };

#define check_state(desired_state ,format, ...)			\
	{	if (g_state_machine != desired_state) 				\
		{  													\
			rtx_dbug_outs((CHAR *) "ERROR in ");	\
			printf(format, ##__VA_ARGS__); 					\
			error("current test state is %d, but should be %d",	\
			g_state_machine, desired_state); 				\
		} 													\
	};
void set_state(int state);
#define get_state()	g_state_machine
/* third party dummy test process 1 */
enum
{
	S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, S12, S13, S14, S15,
};
#define TEST_COMPLETE -1
enum TEST_CASES
{
	GET_SET_PRIORITY = 0,
	REC_REL_MEM_BLK,
	MESSAGES,
	RELEASE_PROCESSOR,
	MEMORY_ORDER,
};
#define NUMBER_OF_TESTS 5
//mutex send and receive
void receive();
void send(int dest_PID);


#endif /* RTX_TEST_COMMON_H_ */

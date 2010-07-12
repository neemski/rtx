/*
 * basic_message.c
 *
 *  Created on: Jul 9, 2010
 *      Author: mzakharo
 */

#include "rtx_inc.h"
#include "rtx_test.h"
#include "rtx_test_common.h"
#define MESSAGE_DATA_SIZE 64
void send_cust_msg(char chr, int pid)
{
	msgEnv_t *oMessage = NULL;
	oMessage = (msgEnv_t*) g_test_fixture.request_memory_block();
	if (oMessage == NULL)
		error("request memory block failed");

	int i;
	for (i = 0; i < MESSAGE_DATA_SIZE; i++)
	{
		oMessage->data[i] = chr;
	}
	if (g_test_fixture.send_message(pid, oMessage) != 0)
		error("sending message failed");

	//printf("sent off a message to %d with %c", pid, chr );

}
void send_cust_msg_to_invalid()
{
	msgEnv_t *oMessage = NULL;
	oMessage = (msgEnv_t*) g_test_fixture.request_memory_block();
	if (oMessage == NULL)
		error("request memory block failed");

	int i;
	for (i = 0; i < MESSAGE_DATA_SIZE; i++)
	{
		oMessage->data[i] = 'z';
	}
	if (g_test_fixture.send_message(-1, oMessage) == 0)
	{
		error("sending message to invalid process succeeded");
	}
	if (g_test_fixture.release_memory_block(oMessage) != 0)
	{
		error("release memory block non-0");
	}
}
void send_cust_delay_msg_to_invalid()
{
	msgEnv_t *oMessage = NULL;

	oMessage = (msgEnv_t*) g_test_fixture.request_memory_block();
	if (oMessage == NULL)
		error("request memory block failed");

	int i;
	for (i = 0; i < MESSAGE_DATA_SIZE; i++)
	{
		oMessage->data[i] = 'z';
	}

	if (g_test_fixture.delayed_send(-1, oMessage, 5) == 0)
	{
		error("sending delayed message to invalid process succeeded");
	}
	if (g_test_fixture.release_memory_block(oMessage) != 0)
	{
		error("release memory block non-0");
	}
}
void send_cust_msg_delay(char chr, int pid, int delay)
{
	msgEnv_t *oMessage = NULL;

	oMessage = (msgEnv_t*) g_test_fixture.request_memory_block();
	if (oMessage == NULL)
		error("request memory block failed");
	int i = 0;
	for (i = 0; i < MESSAGE_DATA_SIZE; i++)
	{
		oMessage->data[i] = chr;
	}
	if (g_test_fixture.delayed_send(pid, oMessage, delay) != 0)
	{
		error("Delayed message send failed");
	}
}
void send_2_delayed(char chr1, char chr2, int pid, int delay)
{
	msgEnv_t *oMessage = NULL;
	msgEnv_t *oMessage2 = NULL;

	oMessage = (msgEnv_t*) g_test_fixture.request_memory_block();
	if (oMessage == NULL)
		error("request memory block failed");

	oMessage2 = (msgEnv_t*) g_test_fixture.request_memory_block();
	if (oMessage2 == NULL)
		error("request memory block failed");

	int i = 0;
	for (i = 0; i < MESSAGE_DATA_SIZE; i++)
	{
		oMessage->data[i] = chr1;
		oMessage2->data[i] = chr2;
	}
	if (g_test_fixture.delayed_send(pid, oMessage, delay) != 0 )
	{
		error("Delayed message send failed");
	}
	if (g_test_fixture.delayed_send(pid, oMessage2, delay) != 0)
	{
		error("Delayed message send failed");
	}
}
void receive_cust_msg(char chrExp, int procExp)
{
	//printf("receiving message");
	int i = 0;
	msgEnv_t *oMessage = NULL;
	oMessage = g_test_fixture.receive_message(&i);
	if (NULL == oMessage)
	{
		error("received NULL pointer as message");
	}
	int j;
	if (i != procExp)
	{
		error("expected different sender process from message");
	}
	for (j = 0; j < MESSAGE_DATA_SIZE; j++)
	{
		if (oMessage->data[j] != chrExp)
		{
			error("expected character %c in message, got %c", chrExp, oMessage->data[j] );
			break;
		}
	}
	//printf("received message with char %c from proc %d", oMessage->data[0], i );
	if (g_test_fixture.release_memory_block(oMessage) != 0)
	{
		error("release memory block non-0");
	}
}

void basic_message_test1()
{
	//"External" testing.
	//Does not test filling up of the queue,
	//or having only one memory element free while passing messages

	g_current_test = MESSAGES;
	send(2);
	printf("send and receive message tests started!");

	int test1_orig_p = g_test_fixture.get_process_priority(1);
	int test2_orig_p = g_test_fixture.get_process_priority(2);
	int test3_orig_p = g_test_fixture.get_process_priority(3);

	g_test_fixture.set_process_priority(1, HIGH );
	g_test_fixture.set_process_priority(2, LOW );
	g_test_fixture.set_process_priority(3, MEDIUM );

	set_state(S10);

	send_cust_msg('a', 2);
	send_cust_msg('b', 2);
	send_cust_msg_delay('c', 2, 2);

	set_state(S11);
	receive_cust_msg('d', 2);
	check_state( S12, "Blocked function ran while waiting for message")

	set_state(S13);

	send_cust_msg('e', 2);
	send_cust_msg('f', 2);
	send_cust_msg_delay('g', 2, 2);
	send_cust_msg_delay('h', 2, 2);
	send_cust_msg_delay('i', 2, 4);
	set_state(S14);
	receive_cust_msg('j', 2);
	receive_cust_msg('k', 2);
	receive_cust_msg('l', 2);
	receive_cust_msg('m', 2);

	check_state( S15, "Blocked function ran while waiting for message" )

	send_cust_msg_to_invalid();
	send_cust_delay_msg_to_invalid();

	printf("Basic message tests passed");
	g_test_fixture.set_process_priority(2, test2_orig_p);
	g_test_fixture.set_process_priority(3, test3_orig_p);
	g_test_fixture.set_process_priority(1, test1_orig_p);
	g_current_test = TEST_COMPLETE;
}
void basic_message_test2()
{

	receive_cust_msg('a', 1);
			receive_cust_msg('b', 1);
			receive_cust_msg('c', 1);
			check_state(S11, "A blocked-on-msg proc ran before receiving messages")

			set_state(S12);
			send_cust_msg('d', 1);

			receive_cust_msg('e', 1);
			receive_cust_msg('f', 1);
			receive_cust_msg('g', 1);
			receive_cust_msg('h', 1);
			receive_cust_msg('i', 1);

			check_state( S14, "A blocked process ran while waiting for messages")

			set_state(S15);
			send_cust_msg_delay('k', 1, 100);
			send_cust_msg_delay('j', 1, 0);
			send_2_delayed('l', 'm', 1, 100);
}

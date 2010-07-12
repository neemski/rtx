/*
 * i_entry.c
 *
 *  Created on: 2010-06-05
 *      Author: mzakharo
 */

#include "i_entry.h"
#include "rtx_inc.h"
#include "dbug.h"
#include "kernel.h"
#include "kernel_globals.h"
#include "i_process.h"

void uart_entry_hanlder();

asm(".text");
asm(".globl uart_entry");
asm("uart_entry:");
DISABLE_INTERRUPTS();
asm("move.l %d0, -(%a7)");
asm("move.l %d1, -(%a7)");
asm("move.l %d2, -(%a7)");
asm("move.l %d3, -(%a7)");
asm("move.l %d4, -(%a7)");
asm("move.l %d5, -(%a7)");
asm("move.l %d6, -(%a7)");
asm("move.l %d7, -(%a7)");
asm("move.l %a0, -(%a7)");
asm("move.l %a1, -(%a7)");
asm("move.l %a2, -(%a7)");
asm("move.l %a3, -(%a7)");
asm("move.l %a4, -(%a7)");
asm("move.l %a5, -(%a7)");
asm("move.l %a6, -(%a7)");
asm("move.l %a7, API_ENTRY_STACK");
asm("jmp uart_entry_handler");

void uart_entry_handler()
{

	messageEnvelope * i_uart_env;
	// malloc instead of request_memory to work on low memory conditions
	i_uart_env = (messageEnvelope *) kernel_malloc(sizeof(messageEnvelope));
	if (i_uart_env != NULL)
	{
		currProc->state = BLK_ISR;
		i_uart_env->message_type = ISR_MSG_TYPE;
		kernel_send_message(I_UART_PID, i_uart_env, 0);
	}
	else
	{
		printf("kernel panic: out of memory, interrupt not processed");
	}

	asm("unlk %fp");
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

void timer0_entry_hanlder();

asm(".text");
asm(".globl timer0_entry");
asm("timer0_entry:");
DISABLE_INTERRUPTS();
asm("move.l %d0, -(%a7)");
asm("move.l %d1, -(%a7)");
asm("move.l %d2, -(%a7)");
asm("move.l %d3, -(%a7)");
asm("move.l %d4, -(%a7)");
asm("move.l %d5, -(%a7)");
asm("move.l %d6, -(%a7)");
asm("move.l %d7, -(%a7)");
asm("move.l %a0, -(%a7)");
asm("move.l %a1, -(%a7)");
asm("move.l %a2, -(%a7)");
asm("move.l %a3, -(%a7)");
asm("move.l %a4, -(%a7)");
asm("move.l %a5, -(%a7)");
asm("move.l %a6, -(%a7)");
asm("move.l %a7, API_ENTRY_STACK");
asm("jmp timer0_entry_handler");

void timer0_entry_handler()
{

	messageEnvelope * i_uart_env;
	// malloc instead of request_memory to work on low memory conditions
	i_uart_env = (messageEnvelope *) kernel_malloc(sizeof(messageEnvelope));
	if (i_uart_env != NULL)
	{
		currProc->state = BLK_ISR;
		i_uart_env->message_type = ISR_MSG_TYPE;
		kernel_send_message(I_TIMER0_PID, i_uart_env, 0);
	}

	asm("unlk %fp");
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

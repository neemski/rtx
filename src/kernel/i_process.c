/*
 * i_process.c
 *
 *  Created on: 2010-06-04
 *      Author: Michael Bieniek
 */

#include "i_process.h"
#include "i_entry.h"
#include "config.h"
#include "dbug.h"
#include "rtx_api.h"
#include "i_uart.h"
#include "kernel_globals.h"
#include "kernel.h"

void i_uart()
{
	BYTE isr;
	BYTE status;
	BYTE uartCharIn;
	BYTE uartCharOut = 0x0;
	messageEnvelope *receivedMsg;
	messageEnvelope *sendEnv;
	int msgSourcePID;

	// Store the uart ISR at user vector #65
	exceptionHandler(65, uart_entry);

	/* Reset the entire UART */
	SERIAL_UCR = 0x10;

	/* Reset the receiver */
	SERIAL_UCR = 0x20;

	/* Reset the transmitter */
	SERIAL_UCR = 0x30;

	/* Reset the error condition */
	SERIAL_UCR = 0x40;

	/* Install the interupt */
	SERIAL_ICR = 0x17; //priority 11
	SERIAL_IVR = 65;

	/* enable interrupts on rx only */
	SERIAL_IMR = 0x02;

	/* Set the baud rate */
	SERIAL_UBG1 = 0x00;
#ifdef _CFSERVER_           /* add -D_CFSERVER_ for cf-server build */
	SERIAL_UBG2 = 0x49; /* cf-server baud rate 19200 */
#else
	SERIAL_UBG2 = 0x92; /* lab board baud rate 9600 */
#endif /* _CFSERVER_ */

	/* Set clock mode */
	SERIAL_UCSR = 0xDD;

	/* Setup the UART (no parity, 8 bits ) */
	SERIAL_UMR = 0x13;

	/* Setup the rest of the UART (noecho, 1 stop bit ) */
	SERIAL_UMR = 0x07;

	/* Setup for transmit and receive */
	SERIAL_UCR = 0x05;
	imr_enable(SERIAL_IMR_MASK);

	while (TRUE)
	{
		receivedMsg = receive_message(&msgSourcePID);
		if (receivedMsg->message_type == SEND_CHAR)
		{
			uartCharOut = receivedMsg->data[0];
			release_memory_block(receivedMsg);
			SERIAL_IMR = 0x3;
		}
		else if (receivedMsg->message_type == ISR_MSG_TYPE)
		{
			free(receivedMsg);// free instead of release to work on low memory conditions
			isr = SERIAL_ISR;
			/* See if data is waiting.... */
			if (isr & MCFUART_UIR_RXREADY)
			{

				while ((status = SERIAL_UCSR) & MCFUART_USR_RXREADY)
				{
					uartCharIn = SERIAL_RD;

#ifdef _DEBUG_HOTKEYS
					if (uartCharIn == '~' || uartCharIn == '!' || uartCharIn
							== '@' || uartCharIn == '#')
						send_hotkey(uartCharIn);
					else
#endif
					{
						sendEnv = (messageEnvelope*) request_memory_block();
						if (sendEnv != NULL)
						{
							sendEnv->data[0] = uartCharIn;
							sendEnv->message_type = DEFAULT_TYPE;
							//		printf("sending '%c' to key dec",sendEnv->data[0]) ;
							send_message(SYS_KEYBOARD_DECODER_PID, sendEnv);
						}
					}
				}
				//print_queue( firstP, lastP, SERIAL_inpQ, QUEUE_SIZE );
			}
			/* See if port is ready to accept data */
			else if (isr & MCFUART_UIR_TXREADY)
			{
				//Receive message.
				//printf("sending character %c", uartCharOut);
				SERIAL_WD = uartCharOut & 0xFF; /* Write data to port */
				// rtx_dbug_outs((CHAR *) "Interrupt write data: ");
				//rtx_dbug_outs((CHAR *) "\r\n");
				SERIAL_IMR = 0x2; /* Disable tx Interupt */
			}
		}

	}
}
#define TIMER0_IMR_MASK 0x1 << 9

void i_timer0()
{
	messageEnvelope *receivedMsg;
	int msgSourcePID;

	// Store the timer0 ISR at auto-vector #6
	exceptionHandler(30, timer0_entry);
	/*
	 * Setup to use auto-vectored interupt level 6, priority 3
	 */
	TIMER0_ICR = 0x9B;
	/*
	 * Set the default reference count
	 and setup the timer0 prescaler and stuff
	 */
#ifdef _EMULATOR_
	TIMER0_TRR = 0x3B;
	TIMER0_TMR = 0x1D;
#else  //1 ms expire
	TIMER0_TRR = 0x3B;
	TIMER0_TMR = 0x2A3D;
#endif
	//unmask the timer interrupt
	imr_enable(TIMER0_IMR_MASK);
	timer0Count = 0;
	while (1)
	{
		receivedMsg = receive_message(&msgSourcePID);
		if (receivedMsg->message_type == ISR_MSG_TYPE)
		{
			free(receivedMsg);
			TIMER0_TER = 2;
			timer0Count++;
			//printf("timer interrupt, time is %d",(int) timer0Count);

			//TODO: perform the while loop of delayed_send queue
			//and send messages to the processes

			messageEnvelope * msgEnv = (messageEnvelope*) timerMailbox.oTail;
			messageEnvelope * msgTmp = NULL;
			while (msgEnv != NULL)
			{
				if (msgEnv->send_time == timer0Count)
				{
					//Case 1: beginning of the list
					if (msgEnv->oPrev == NULL)
					{
						//only one item left
						if (msgEnv->oNext == NULL)
						{
							timerMailbox.oHead = NULL;
							timerMailbox.oTail = NULL;
						}
						else
						{
							timerMailbox.oHead =  msgEnv->oNext;
							timerMailbox.oHead->oPrev = NULL;
						}
					}
					//Case 2: Found PCB at end of list.
					else if (msgEnv->oNext == NULL)
					{
						timerMailbox.oTail = msgEnv->oPrev;
						timerMailbox.oTail->oNext = NULL;
					}
					//Case 3: Found PCB in the middle.
					else
					{
						msgEnv->oPrev->oNext = msgEnv->oNext;
						msgEnv->oNext->oPrev = msgEnv->oPrev;
					}
					msgTmp = msgEnv->oPrev;
					//send that thing off
					if (kernel_send_message(msgEnv->destination_process_id,
							msgEnv, 1) == RTX_ERROR)
					{
						//This shounldn't ever happen anyways if processes don't terminate
						printf("kernel panic - delayed send message failed");
						kernel_release_memory_block(msgEnv);
					}
					msgEnv = msgTmp;
				}
				else
					msgEnv = msgEnv->oPrev;
			}

		}

	}

}


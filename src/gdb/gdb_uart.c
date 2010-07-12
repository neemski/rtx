/**
 * @file: gdb_uart.c
 * @brief: gdb_uart/second gdb_uart port/rtx terminal I/O sample code
 * @author: ECE Lab Instructors and TAs
 * @author: Irene Huang
 * @date: 2010/05/03
 */
#include "rtx_inc.h"
#include "dbug.h"
#include "queue.h"
#include "rtx_api.h"

/*
 * Global Variables
 */

#define QUEUE_SIZE 100
char serial_inpQ[QUEUE_SIZE];
volatile char * firstP;
volatile char * lastP;
extern void asm_gdb_uart_entry();

asm(".text");
asm(".globl asm_gdb_uart_entry");
asm("asm_gdb_uart_entry:");
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
asm("jsr c_gdb_uart_handler");
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

/*
 * This function is called by the assembly STUB function
 */
VOID c_gdb_uart_handler(VOID )
{
	BYTE isr;
	BYTE status;
	BYTE CharIn;
	isr = SERIAL1_ISR;
	/* See if data is waiting.... */
	if (isr & MCFUART_UIR_RXREADY)
	{

		while ((status = SERIAL1_UCSR) & MCFUART_USR_RXREADY)
		{
			CharIn = SERIAL1_RD;
			if (status & MCFUART_USR_RXERR)
			{
				SERIAL1_UCR = MCFUART_UCR_CMDRESETERR;
			}
#ifdef _GDB_UART_DEBUG_
			//rtx_dbug_outs((CHAR *) "Interrupt read data: ");
			rtx_dbug_out_char(CharIn);
			//rtx_dbug_outs((CHAR *) "\r\n");
#endif 
			insert_to_queue(CharIn, &firstP, &lastP, serial_inpQ, QUEUE_SIZE);
		}
		//print_queue( firstP, lastP, serial_inpQ, QUEUE_SIZE );
	}
	/* See if port is ready to accept data */
	else if (isr & MCFUART_UIR_TXREADY)
	{
		//		rtx_dbug_outs((CHAR *) "Interrupt write data: ");
		//rtx_dbug_outs((CHAR *) "\r\n");
		SERIAL1_IMR = 2; /* Disable tx Interupt */
	}
	return;
}

int gdb_uart_init()
{
	/*
	 * Store the gdb_uart ISR at user vector #64
	 */

	exceptionHandler(64, asm_gdb_uart_entry);
	/* Reset the entire UART */
	SERIAL1_UCR = 0x10;

	/* Reset the receiver */
	SERIAL1_UCR = 0x20;

	/* Reset the transmitter */
	SERIAL1_UCR = 0x30;

	/* Reset the error condition */
	SERIAL1_UCR = 0x40;

	/* Install the interupt */
	SERIAL1_ICR = 0x17; //priority 11
	SERIAL1_IVR = 64;

	/* enable interrupts on rx only */
	SERIAL1_IMR = 0x02;

	/* Set the baud rate */
	SERIAL1_UBG1 = 0x00;
#ifdef _CFSERVER_           /* add -D_CFSERVER_ for cf-server build */
	SERIAL1_UBG2 = 0x49; /* cf-server baud rate 19200 */
#else
	SERIAL1_UBG2 = 0x92; /* lab board baud rate 9600 */
#endif /* _CFSERVER_ */

	/* Set clock mode */
	SERIAL1_UCSR = 0xDD;

	/* Setup the UART (no parity, 8 bits ) */
	SERIAL1_UMR = 0x13;

	/* Setup the rest of the UART (noecho, 1 stop bit ) */
	SERIAL1_UMR = 0x07;

	/* Setup for transmit and receive */
	SERIAL1_UCR = 0x05;

	/*initialize FIFO buffer*/
	firstP = serial_inpQ;
	lastP = firstP;
	imr_enable(SERIAL1_IMR_MASK);

	return 0;
}
//print one character on the gdb_uart port using pollling
void gdb_uart_out_char(CHAR c)
{
	int i;
	for (i = 0; (i < 0x10000); i++)
	{
		if (SERIAL1_UCSR & MCFUART_USR_TXREADY)
			break;
	}
	SERIAL1_WD = c & 0xFF; /* Write data to port */
	for (i = 0; (i < 0x10000); i++)
	{
		if (SERIAL1_UCSR & MCFUART_USR_TXREADY)
			break;
	}
}
//get one character from the gdb_uart port
CHAR gdb_uart_in_char()
{
	CHAR temp;
	temp = pop_from_queue(&firstP, &lastP, serial_inpQ, QUEUE_SIZE);
	//printf("gdb_uart_in char %c", temp);
	return temp;
}


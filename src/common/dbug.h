/**
 * @file: dbug.h
 * @author: ECE354 Lab Instructors and TAs
 * @author: Irene Huang  
 * @date: 2010/05/03
 * @brief: Header file of dbug.c.  
 */
#ifndef _DBUG_H_
#define _DBUG_H_

#include "rtx_inc.h"

VOID uart_out_char(CHAR c); //output a char to a uart device

VOID rtx_dbug_out_char(CHAR c); //output a char to uart0 device

SINT32 rtx_dbug_outs(CHAR* s); // output a string to janusROM terminal
int printf(const char *format, ...);


#define BREAKPOINT() asm("trap #1")
#endif /* _DBUG_H_ */

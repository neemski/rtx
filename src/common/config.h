/*
 * config.h
 *
 *  Created on: 2010-05-31
 *      Author: mzakharo
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define MEMORY_BLOCK_SIZE 128
#define NUMBER_OF_MEMORY_BLOCKS 30

//to add a new process, create a define here and use it in process.c

//user processes
#define WALL_CLOCK
#define SET_PRIORITY

#define USER_PROC_A
#define USER_PROC_B
#define USER_PROC_C

//system processes
#define KCD 
#define CRT
#define I_UART
#define I_TIMER0
//should always be here
#define NULL_PROC

//enum of all RTX processes and their PIDs
enum
{
	NULL_PID = 0,
	USER_PROC_A_PID = 7,
	USER_PROC_B_PID,
	USER_PROC_C_PID,
	I_UART_PID,
	I_TIMER0_PID,
	SYS_KEYBOARD_DECODER_PID,
	SYS_CRT_DISPLAY_PID,
	USER_WALL_CLK_DISPLAY_PID,
	USER_SET_PRIORITY_PID,
};

//priority level enums
enum
{
	HIGH,
	MEDIUM,
	LOW,
	LOWEST,
	NULL_P,
};
#define NUMBER_OF_PRIORITIES NULL_P + 2
#endif /* CONFIG_H_ */

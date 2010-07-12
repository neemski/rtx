/*
 * i_entry.h
 *
 *  Created on: 2010-05-28
 *      Author: mzakharo
 */

#ifndef API_ENTRY_H_
#define API_ENTRY_H_
#include "rtx_inc.h"
VOID api_entry(VOID);

enum isr_function
{
	PROC_SWITCH_F,
	MALLOC_F,
	KERNEL_FREE_F,
	REQ_MEMBLOCK_F,
	RELEASE_MEMBLOCK_F,
	SEND_MSG_F,
	RCV_MSG_F,
	RELEASE_PROCESSOR_F,
	HOTKEY_F,
	SEND_DELAYED_MSG_F,
	COUNT_MEM_BLKS_F,
	SET_PROCESS_PRIORITY,
	GET_PROCESS_PRIORITY,
	GET_TICK_COUNT,
};

#endif /* I_ENTRY_H_ */

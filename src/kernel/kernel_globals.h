/*
 * kernel_globals.h
 *
 *  Created on: 2010-05-30
 *      Author: mzakharo
 */

#ifndef KERNEL_GLOBALS_H_
#define KERNEL_GLOBALS_H_
#include "pcb.h"
#include "memory.h"
#include "priority_queue.h"
#include "rtx_api.h"
UINT32 API_ENTRY_STACK; /*used for context saving at API entry */

extern int _end;
/*master memory pointer*/
memory_block *mem_list_head;
/*simple allocated pointer for the blocks used for this project */
memory_block *rtx_mem_list_head;

//kernel states
priority_queue state_q;
pcb* currProc;
pcb* blkOnIsrProc;

msg_queue timerMailbox;
UINT32 timer0Count;

#endif /* KERNEL_GLOBALS_H_ */

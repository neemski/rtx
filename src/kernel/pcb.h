/*
 * pcb.h
 *
 *  Created on: 2010-05-28
 *      Author: mzakharo
 */

#ifndef PCB_H_
#define PCB_H_
#include "rtx_inc.h"
#include "rtx_queue.h"
typedef struct pcb
{
	struct pcb *oNext;
	struct pcb *oPrev;
	int pid; //Needed as int because of the requirements (see message queue)
	signed int priority;
	UINT32 stack_pointer;
	Queue mailBox;
	int procType;
	int state;
} pcb;


enum PROC_STATES
{
	READY,
	SWITCH,
	BLK_ISR,
	BLK_MSG,
	BLK_MEM,
};
#endif /* PCB_H_ */

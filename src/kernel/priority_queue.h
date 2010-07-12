/*
 * priority_queue.h
 *
 *  Created on: 2010-06-04
 *      Author: Michael Bieniek
 */


#ifndef PRIORITY_QUEUE_H_
#define PRIORITY_QUEUE_H_
#include "pcb.h"
#include "config.h"
#include "rtx_queue.h"
typedef struct priority_level
{
	struct pcb * oHead;
	struct pcb * oTail;

}priority_level;
typedef struct priority_queue
{
	priority_level bins[NUMBER_OF_PRIORITIES];

}priority_queue;


pcb *pop_pid(priority_queue *queue,UINT32 nPid);
pcb *pop(priority_queue *queue);
void push(priority_queue *queue,pcb *oPcb);
void push_to_front(priority_queue *queue, pcb *oPcb);
pcb *find(priority_queue *queue,UINT32 nPid);
void init_queue(priority_queue *queue);
void print_pqueue(priority_queue *queue, int state);





#endif /* PRIORITY_QUEUE_H_ */

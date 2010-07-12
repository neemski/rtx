/*
 * rtx_queue.h
 *
 *  Created on: Jul 11, 2010
 *      Author: mzakharo
 */

#ifndef RTX_QUEUE_H_
#define RTX_QUEUE_H_



typedef struct QueueNode
{
	void * oNext;
	void * oPrev;
} QueueNode;

typedef struct QueueStruct
{
	struct QueueNode * oHead;
	struct QueueNode * oTail;
} Queue;

void init_rtx_queue(void * oQueue);
void push_rtx_queue(void * oQueue, void * obj);
void * pop_rtx_queue( void * oQueue);
#endif /* RTX_QUEUE_H_ */

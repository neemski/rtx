/*
 * rtx_queue.c
 *
 *  Created on: Jul 11, 2010
 *      Author: mzakharo
 */
#include "rtx_queue.h"
#include "rtx_inc.h"
void init_rtx_queue(void * queue)
{

	Queue * oQueue  =(Queue*)  queue;
	oQueue->oHead = NULL;
	oQueue->oTail = NULL;
}

void push_rtx_queue(void * queue, void * obj)
{
	Queue * oQueue  =(Queue*)  queue;

	QueueNode *oObj = (QueueNode *)obj;
	//empty  queue
	if (oQueue->oHead == NULL)
	{
		oObj->oNext = NULL;
		oObj->oPrev = NULL;
		oQueue->oHead = oObj;
		oQueue->oTail = oObj;
	}
	else
	{
		oQueue->oHead->oPrev = oObj;
		oObj->oNext = oQueue->oHead;
		oObj->oPrev = NULL;
		oQueue->oHead = oObj;
	}
}
void * pop_rtx_queue(void * queue)
{

	Queue * oQueue  =(Queue*)  queue;
	QueueNode *oRet = NULL;
	//Save pointer to last pcb.
	oRet = oQueue->oTail;
	//Special case: when only one item in list.
	if (oRet == oQueue->oHead)
	{
		oQueue->oHead = NULL;
		oQueue->oTail = NULL;
	}
	else
	{
		//Reassign tail pointer.
		oQueue->oTail = oRet->oPrev;
		oQueue->oTail->oNext= NULL;
	}
	return oRet;
}

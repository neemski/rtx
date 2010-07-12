/*
 * priority_queue.c
 *
 *  Created on: 2010-06-04
 *      Author: Michael Bieniek
 */

#include "priority_queue.h"
#include "dbug.h"
#include "process.h"
#include "rtx_api.h"

pcb *pop(priority_queue *queue)
{
	int nPriority = 0;
	while (nPriority < NUMBER_OF_PRIORITIES)
	{
		priority_level * bin = &queue->bins[nPriority];
		//Check to see if the current priority level is populated.
		if (bin->oTail != NULL)
		{
			return (pcb *)pop_rtx_queue((Queue *)bin);
		}
		nPriority++;
	}
	return NULL;
}

pcb *pop_pid(priority_queue *queue, UINT32 nPid)
{
	int i;
	pcb * tmp = NULL;
	priority_level * bin;
	for (i = 0; i < NUMBER_OF_PRIORITIES; i++)
	{
		tmp = (pcb *) queue->bins[i].oHead;
		while (tmp != NULL)
		{

			if (tmp->pid == nPid)
				break;
			tmp = tmp->oNext;

		}
		if ( tmp != NULL && tmp->pid == nPid)
			break;
	}
	if (tmp == NULL)
		return NULL;
	bin = &queue->bins[i];
	//Case 1: beginning of the list
	if (tmp->oPrev == NULL)
	{
		//only one item left
		if (tmp->oNext == NULL)
		{
			bin->oHead = NULL;
			bin->oTail = NULL;
		}
		else
		{
			bin->oHead = (void *) tmp->oNext;
			bin->oHead->oPrev = NULL;
		}
	}
	//Case 2: Found PCB at end of list.
	else if (tmp->oNext == NULL)
	{
		bin->oTail = (void *) tmp->oPrev;
		bin->oTail->oNext = NULL;
	}
	//Case 3: Found PCB in the middle.
	else
	{
		tmp->oPrev->oNext = tmp->oNext;
		tmp->oNext->oPrev = tmp->oPrev;
	}

	return tmp;
}

void push(priority_queue *queue, pcb *oPcb)
{
	priority_level * bin = &queue->bins[oPcb->priority + 1];
	//empty bin
	push_rtx_queue((Queue *)bin, oPcb);
}
pcb *find(priority_queue *queue, UINT32 nPid)
{
	int i;
	pcb * tmp;
	for (i = 0; i < NUMBER_OF_PRIORITIES; i++)
	{
		tmp = (pcb *) queue->bins[i].oHead;
		while (tmp != NULL)
		{
			if (tmp->pid == nPid)
				return tmp;
			tmp = tmp->oNext;
		}
	}
	return NULL;
}

void init_queue(priority_queue *queue)
{
	//Traverse through and set all properties to NULL.
	int nCount = 0;
	while (nCount < NUMBER_OF_PRIORITIES)
	{
		init_rtx_queue((Queue *)&queue->bins[nCount]);
		nCount++;
	}
	return;
}

void print_pqueue(priority_queue *queue, int state)
{
	int i;
	pcb * tmp;
	if (queue == NULL)
		return;
	for (i = 0; i < NUMBER_OF_PRIORITIES; i++)
	{
		tmp = (pcb *) queue->bins[i].oHead;
		while (tmp != NULL)
		{
			if (state == tmp->state)
			{
				printf("PCB ID is %d (%s), priority %d", tmp->pid,
						pid_to_string(tmp->pid), tmp->priority);
			}
			tmp = tmp->oNext;
		}
	}
}

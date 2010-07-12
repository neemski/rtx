/*
 * rtx_api.h
 *
 *  Created on: 2010-05-28
 *      Author: mzakharo
 */

#ifndef RTX_API_H_
#define RTX_API_H_
#include "rtx_inc.h"
#include <stddef.h>
#include "messageEnvelope.h"
#include "rtx_queue.h"

/**************************************************************************
 The RTX will support a message-based IPC discussed in lectures. Messages are
 carried in envelopes (memory blocks, see below) with a header which is less
 than 64 bytes. Two IPC primitives will be implemented. The two primitives are:
 **************************************************************************/

/**************************************************************************
 Delivers to the destination process a message carried in the message envelope
 (a memory block). Changes the state of destination process to ready_to_execute
 if appropriate. The sending process is preempted if the receiving process was
 blocked waiting for a message and has higher priority, otherwise the sender
 continues executing. The header of the message will have the layout given in
 the course overheads. It also fills in the sender_process_id and
 destina-tion_process_id fields in the message envelope. The fields
 sender_process_id, destination_process_id and mes-sage_type are all
 of type int. The sender fills in the message_type field of the message
 envelope before invoking the primitive.
 **************************************************************************/
int send_message(int process_ID, void * MessageEnvelope);

/**************************************************************************/
/*This is a blocking receive. If there is a message waiting,
 a pointer to the message envelope containing it will be re-turned
 to the caller. If there is no such message, the calling process
 blocks and another process is selected for execu-tion. The sender
 of the message is identified through sender_ID, unless it is NULL.
 Note the sender_ID is an output parameter and is not meant to filter
 which message to receive.     */
/**************************************************************************/
void * receive_message(int * sender_ID);

/**************************************************************************/
/*The RTX supports a simple memory management scheme. The memory is divided
 into blocks of fixed size (128 bytes minimum). The size and the number of
 these blocks is a configuration parameter. The blocks can be used by the
 request-ing processes for storing local variables or as envelopes for messages
 sent to other processes. A block which is no long-er needed must be returned
 to the RTX. Two primitives are to be provided:*/
/**************************************************************************/

/**************************************************************************/
/*The primitive returns a pointer to a memory block
 to the calling process. If no memory block is available, the calling process is
 blocked until a memory block becomes available. If several processes are waiting
 for a memory block and a block becomes available, the highest priority waiting
 process will get it.*/
/**************************************************************************/
void * request_memory_block();

/**************************************************************************
 This primitive returns the memory
 block to the RTX. If there are processes waiting for a block, the block is given
 to the highest priority process, which is then unblocked. The caller of this
 primitive never blocks, but could be preempted. Thus, it may affect the currently
 executing process.
 **************************************************************************/
int release_memory_block(void * MemoryBlock);

/**************************************************************************
 first free list implementation of malloc
 **************************************************************************/
void * malloc(size_t size);
/**************************************************************************
 first free list implementation of free - with simple defragmentation support
 **************************************************************************/
void free(void* ptr);

/**************************************************************************
 Control is transferred to the RTX (the calling process voluntarily releases
 the processor). The invoking process re-mains ready to execute. Another
 process may possibly be selected for execution.
 **************************************************************************/
int release_processor();

/**************************************************************************
 * registers a function for a given interrupt vector
 **************************************************************************/
VOID exceptionHandler(BYTE interruptVector, VOID * address);

void imr_enable(int bit);
void send_hotkey(BYTE hotkey);
int count_mem(int);


char * pid_to_string(int pid);
/*Timing Service*/
int delayed_send(int process_ID, void * MessageEnvelope, int delay);
int get_process_priority (int process_ID);
int set_process_priority (int process_ID, int priority);

UINT32 get_tick_count();

#endif /* RTX_API_H_ */

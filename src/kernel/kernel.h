/*
 * kernel.h
 *
 *  Created on: 2010-05-28
 *      Author: mzakharo
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include "rtx_inc.h"
#include "memory.h"
#include "pcb.h"

/**************************************************************************
 * switches to a different process by pid
 * if the process with a given pid does not exist, returns without change
 * of context
 **************************************************************************/
VOID kernel_proc_switch(pcb * nextProc);
/**************************************************************************
 * prepares software interrupts, sets up the process context
 **************************************************************************/
VOID kernel_init();
/**************************************************************************
 The RTX will support a message-based IPC discussed in lectures. Messages are
 carried in envelopes (memory blocks, see below) with a header which is less
 than 64 bytes. Two IPC primitives will be implemented. The two primitives are:
 **************************************************************************/

/*
 * The invoking process does not block. The message (in the memory block pointed to by the second parameter) will
be sent to the destination process (process_ID) after the expiration of the delay (timeout, given in msec units).
 */
int kernel_delayed_send( int process_ID, void * MessageEnvelope, int delay );

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
int kernel_send_message( int process_ID, void * MessageEnvelope, int useMessagePID );

/**************************************************************************/
/*This is a blocking receive. If there is a message waiting,
 a pointer to the message envelope containing it will be re-turned
 to the caller. If there is no such message, the calling process
 blocks and another process is selected for execu-tion. The sender
 of the message is identified through sender_ID, unless it is NULL.
 Note the sender_ID is an output parameter and is not meant to filter
 which message to receive.     */
/**************************************************************************/
void * kernel_receive_message(int * sender_ID);

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
void * kernel_request_memory_block();

/**************************************************************************
 This primitive returns the memory
 block to the RTX. If there are processes waiting for a block, the block is given
 to the highest priority process, which is then unblocked. The caller of this
 primitive never blocks, but could be preempted. Thus, it may affect the currently
 executing process.
 **************************************************************************/
int kernel_release_memory_block(void * MemoryBlock);

/**************************************************************************
 Control is transferred to the RTX (the calling process voluntarily releases
 the processor). The invoking process re-mains ready to execute. Another
 process may possibly be selected for execution.
 **************************************************************************/
int kernel_release_processor();
void  scheduler();
void kernel_send_hotkey(BYTE hotkey);
void init_vbr();

int kernel_get_process_priority (int process_ID);
int kernel_set_process_priority (int process_ID, int priority);

UINT32 kernel_get_timer0_tick();

#endif /* KERNEL_H_ */

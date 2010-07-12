/*
 * messageEnvelope.h
 *
 *  Created on: Jun 22, 2010
 *      Author: Vlad
 */

#ifndef MESSAGEENVELOPE_H_
#define MESSAGEENVELOPE_H_
#include "config.h"
/*the message struct that all processes can use and pass
 *
 */


#define MESSAGE_MEMBERS	struct messageEnvelope * oNext; \
						struct messageEnvelope * oPrev; \
						int message_type; \
						int sender_process_id; \
						int destination_process_id; \
						int msg_length; \
						unsigned int send_time;
struct messagePhony
{
	MESSAGE_MEMBERS
};

#define MESSAGE_DATA_SIZE MEMORY_BLOCK_SIZE - sizeof(struct messagePhony)
typedef struct messageEnvelope
{
	MESSAGE_MEMBERS
	BYTE data[MESSAGE_DATA_SIZE];
} messageEnvelope;


enum MESSAGE_TYPES
{
	DEFAULT_TYPE,
	ISR_MSG_TYPE = 1,
	SEND_CHAR,
	REGISTER_CMD,
	CMD_REQUEST,
	WALL_CLOCK_TICK,
	COUNT_REPORT,
	WAKEUP_10,
};
typedef struct msg_queue
{
	messageEnvelope * oHead;
	messageEnvelope * oTail;
}msg_queue;

#endif /* MESSAGEENVELOPE_H_ */

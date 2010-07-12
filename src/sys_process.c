/*
 * sys_process.c
 *
 *  Created on: 2010-06-06
 *      Author: Michael Bieniek
 */
#include "sys_process.h"
#include "config.h"
#include "dbug.h"
#include "rtx_api.h"
#include "utils.h"
#define CMD_BUFFER_SIZE MESSAGE_DATA_SIZE
#undef printf
#define printf(format, ...){rtx_dbug_outs((CHAR *) "rtx_syst: ");\
							printf(format, ## __VA_ARGS__); };


typedef struct reg_cmd_name
{
	char cCommand;
	int nDestinationPid;
	struct reg_cmd_name *oNext;

} reg_cmd_name;

void sys_keyboard_decoder()
{
	messageEnvelope *oRecMsg = NULL;
	messageEnvelope *oSendCrtMsg;
	reg_cmd_name *oRegCmdHead = NULL;

	BYTE cRecData;
	BYTE bCmdSense = 0;
	char cCmdBuffer[CMD_BUFFER_SIZE];
	int nBufferIndex = 0;
	int nRecPid;
	int nRecType;
	//Busy loop.
	while (1)
	{
		//Receive message.
		oRecMsg = receive_message(&nRecPid);

		//Save message contents.
		cRecData = oRecMsg->data[0];
		nRecType = oRecMsg->message_type;

		release_memory_block(oRecMsg);
		//printf("Keyboard decoder received: %c", cRecievedData);
		//Check if the message is a command registration message.
		if (nRecType == REGISTER_CMD)
		{
			printf("recieved register");

			//Check if the registered command list is empty.
			if (oRegCmdHead == NULL)
			{
				oRegCmdHead = (reg_cmd_name*) malloc(sizeof(reg_cmd_name));
				if (oRegCmdHead != NULL)
				{
					oRegCmdHead->cCommand = cRecData;
					oRegCmdHead->nDestinationPid = nRecPid;
					oRegCmdHead->oNext = NULL;
				}
				else
				{
					printf("Cannot register cmd %c: out of memory", cRecData);
				}
			}
			else
			{
				//See if the command exists. If so, override the receiving PID.
				reg_cmd_name *oSearchPtr = oRegCmdHead;
				while (oSearchPtr != NULL)
				{
					if (oSearchPtr->cCommand == cRecData)
					{
						oSearchPtr->nDestinationPid = nRecPid;
						break;
					}
					oSearchPtr = oSearchPtr->oNext;
				}
				//Reach the end of the list.
				if (oSearchPtr == NULL)
				{
					//replace the head
					reg_cmd_name *oPointer = (reg_cmd_name*) malloc(
							sizeof(reg_cmd_name));
					if (oPointer != NULL)
					{
						oPointer->cCommand = cRecData;
						oPointer->nDestinationPid = nRecPid;
						oPointer->oNext = oRegCmdHead;
						oRegCmdHead = oPointer;
					}
					else
					{
						printf("Cannot register cmd %c: out of memory",
								cRecData);
					}
				}

			}
			printf("registered cmd: '%c' for %s", cRecData, pid_to_string(
					nRecPid));
		}

		else
		{
			//Check if the received message starts with a %
			if (bCmdSense == 0 && cRecData == '%')
			{
				bCmdSense = 1;
				nBufferIndex = 0;
			}
			//Add character to buffer.

			else if (bCmdSense == 1)
			{
				//Check if the character is a CR.
				if (cRecData == CR || cRecData == LF)
				{
					//Send message to resulting process.
					cCmdBuffer[nBufferIndex] = '\0';
					bCmdSense = 0;
					//Find the command.
					reg_cmd_name *oCmdPointer = oRegCmdHead;
					while (oCmdPointer != NULL)
					{
						//Find location and send message.
						if (oCmdPointer->cCommand == cCmdBuffer[0])
						{

							messageEnvelope *oRegCmdMsg;
							oRegCmdMsg
									= (messageEnvelope*) request_memory_block();
							if (oRegCmdMsg != NULL)
							{
								strcpy((char *) oRegCmdMsg->data,
										(char *) cCmdBuffer);
								oRegCmdMsg->message_type = CMD_REQUEST;
								send_message(oCmdPointer->nDestinationPid,
										oRegCmdMsg);
							}
							break;
						}
						oCmdPointer = oCmdPointer->oNext;
					}
				}
				//Check for too much information.

				else if (nBufferIndex < CMD_BUFFER_SIZE - 1)
				{
					cCmdBuffer[nBufferIndex] = cRecData;
					nBufferIndex++;
				}
			}
			//Send to CRT display process.
			oSendCrtMsg = (messageEnvelope*) request_memory_block();
			if (oSendCrtMsg != NULL)
			{
				oSendCrtMsg->data[0] = cRecData;
				if (cRecData == CR)
				{
					oSendCrtMsg->data[1] = LF;
					oSendCrtMsg->data[2] = '\0';
				}
				else
					oSendCrtMsg->data[1] = '\0';
				//printf("sending '%s' to CRT",oSendCrtMsg->data);
				send_message(SYS_CRT_DISPLAY_PID, oSendCrtMsg);
			}
		}
	}
}

void sys_crt_display()
{
	messageEnvelope *oMsg = NULL;
	char buffer[MESSAGE_DATA_SIZE];
	int i;
	//Busy loop.
	while (1)
	{
		oMsg = receive_message(NULL);
		strcpy(buffer, (char *) oMsg->data);
		release_memory_block(oMsg);
		for (i = 0; buffer[i] != '\0'; ++i)
		{
			oMsg = request_memory_block();
			if (oMsg != NULL)
			{
				oMsg->data[0] = buffer[i];
				oMsg->message_type = SEND_CHAR;
				//printf("sending '%s' to UART", oMsg->data);
				send_message(I_UART_PID, oMsg);
			}
		}
	}
}

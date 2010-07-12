/*
 * user_process.c
 *
 *  Created on: Jun 21, 2010
 *      Author: Nima Dehnashi
 */

#include "config.h"
#include "dbug.h"
#include "rtx_api.h"
#include "utils.h"

void user_wall_clk_display()
{
	messageEnvelope *oMsg = NULL;
	messageEnvelope *oSendCrtMsg = NULL;
	messageEnvelope delayedMsg;
	char buffer[MESSAGE_DATA_SIZE];
	int nRecType;

	int nWCDInit = 0;

	int nHrs = 0;
	int nMins = 0;
	int nSecs = 0;

	UINT32 oldTicks = 0;
	UINT32 newTicks = 0;

	// Register the WCD with the KCD to handle commands starting with '%W'
	oMsg = (messageEnvelope*) request_memory_block();
	oMsg->data[0] = 'W';
	oMsg->message_type = REGISTER_CMD;
	send_message(SYS_KEYBOARD_DECODER_PID, oMsg);

	delayedMsg.message_type = WALL_CLOCK_TICK;
	delayed_send(USER_WALL_CLK_DISPLAY_PID, &delayedMsg, 1000);

	while (1)
	{

		//Receive message.
		oMsg = receive_message(NULL);
		//Save message contents.
		nRecType = oMsg->message_type;

		if (nRecType == CMD_REQUEST)
		{
			// If first two input characters are == "WS", set WCD
			if (strncmp((char *) oMsg->data, "WS", 2) == 0)
			{
				// Check if it's equal to a space
				if (oMsg->data[2] == ' ')
				{
					if ((((oMsg->data[3] == '0') || (oMsg->data[3] == '1'))
							&& ((oMsg->data[4] >= 48) && (oMsg->data[4] <= 57)))
							|| ((oMsg->data[3] == 50) && ((oMsg->data[4] >= 48)
									&& (oMsg->data[4] <= 51))))
					{
						// Check if it's equal to a colon
						if (oMsg->data[5] == 58)
						{
							if (((oMsg->data[6] >= 48) && (oMsg->data[6] <= 53))
									&& ((oMsg->data[7] >= 48) && (oMsg->data[7]
											<= 57)))
							{
								// Check if it's equal to a colon
								if (oMsg->data[8] == 58)
								{
									if (((oMsg->data[9] >= 48)
											&& (oMsg->data[9] <= 53))
											&& ((oMsg->data[10] >= 48)
													&& (oMsg->data[10] <= 57))
											&& (oMsg->data[11] == '\0'))
									{

										nHrs = ( oMsg->data[3] - 48) * 10 + ( oMsg->data[4] - 48);
										nMins = ( oMsg->data[6] - 48) * 10 + ( oMsg->data[7] - 48);
										nSecs = ( oMsg->data[9] - 48) * 10 + ( oMsg->data[10] - 48);
										oldTicks = get_tick_count();
										newTicks = oldTicks;
										nWCDInit = 1;

										printf("Wall clock initialized.");
									}
									else
									{
										printf(
												"Wrong format for wall clock input. Please enter seconds ranging from [00-59].");
									}
								}
								else
								{
									printf(
											"Wrong format for wall clock input. Please enter a colon after the specified minutes.");
								}
							}
							else
							{
								printf(
										"Wrong format for wall clock input. Please enter minutes ranging from [00-59].");
							}
						}
						else
						{
							printf(
									"Wrong format for wall clock input. Please enter a colon after the specified hours.");
						}
					}
					else
					{
						printf(
								"Wrong format for wall clock.  Please enter a time in 24-hour format.");
					}
				}
				else
				{
					printf(
							"Wrong format for wall clock input. Please enter a space after 'WS', then specify the time.");
				}
			}
			// If the input command is equal to "T", terminate the WCD
			else if (strncmp((char *) oMsg->data, "WT", 3) == 0)
			{
				//Terminate the WCD
				printf("Wall clock process terminated.");
				nWCDInit = 0;
			}
			else
			{
				printf("The wall clock command entered is unrecognized.");
			}
			release_memory_block(oMsg);
		}
		else if (nRecType == WALL_CLOCK_TICK)
		{
			delayed_send(USER_WALL_CLK_DISPLAY_PID, oMsg, 1000);
			if (nWCDInit == 1)
			{
				if( nSecs > 59 )
				{
					nSecs = 0;
					nMins++;
					if( nMins > 59 )
					{
						nMins = 0;
						nHrs++;
						if( nHrs > 23 )
						{
							nHrs = 0;
						}
					}
				}

				// Display format of the clock stored in buffer
				sprintf(buffer, "%02d:%02d:%02d\r", nHrs, nMins, nSecs );
				//printf("buffer is %s",buffer);
//#define OLD_WALLCLOCK_STYLE

#ifdef OLD_WALLCLOCK_STYLE
				nSecs++;
#else
				newTicks = get_tick_count();
				UINT32 diffTicks = newTicks - oldTicks;

				/* Integer division always rounds down.
				 * if diffTicks is xxx499 and below then round down,
				 *  if it's yyy500 and above - round up. Mod 60 makes sure that no overflows occur */
				nSecs += ( ( diffTicks + 500) / 1000 ) % 60;
				/* just in case... */
				nMins += ( (diffTicks + 30000 ) / 60000 ) % 60;
				nHrs += ( (diffTicks + 1800000 ) / 3600000 ) % 60;

				//printf("Ticks is %d", (int)diffTicks );
				oldTicks = newTicks;
#endif
				//printf("buffer is '%s'", buffer);
				oSendCrtMsg = (messageEnvelope*) request_memory_block();

				// ANSI code to position the clock near the top-right of the terminal window
				sprintf((char *) oSendCrtMsg->data, "%c[s%c[1;32H%s%c[u", ESC,
						ESC, buffer, ESC );
				send_message(SYS_CRT_DISPLAY_PID, oSendCrtMsg);
			}
		}

	}
}

void user_set_priority()
{
	messageEnvelope *oMsg = NULL;
	int nRecType;

	int nProcess_id = 0;
	int nNew_priority = 0;

	// Register with the KCD to handle commands starting with '%C'
	oMsg = (messageEnvelope*) request_memory_block();
	oMsg->data[0] = 'C';
	oMsg->message_type = REGISTER_CMD;
	send_message(SYS_KEYBOARD_DECODER_PID, oMsg);

	while (1)
	{
		//Receive message.
		oMsg = receive_message(NULL);
		//Save message contents.
		nRecType = oMsg->message_type;

		if (nRecType == CMD_REQUEST)
		{
			//Check if format is preserved, 2nd/4th character should be a space last character should be null
			if ((oMsg->data[1] == 32) && (oMsg->data[3] == 32)
					&& (oMsg->data[5] == '\0'))
			{
				//Process id's range from [1-9]
				//Check if request corresponds with existing PIDs
				if ((oMsg->data[2] > (NULL_PID + 48)) && (oMsg->data[2]
						< (I_UART_PID + 48)))
				{
					//New priority levels should be between [0-3]
					if ((oMsg->data[4] >= (HIGH + 48)) && (oMsg->data[4]
							<= (LOWEST + 48)))
					{
						nProcess_id = ((int) oMsg->data[2]) - 48;
						nNew_priority = ((int) oMsg->data[4]) - 48;

						set_process_priority(nProcess_id, nNew_priority);
					}
					else
					{
						printf(
								"Illegal request for new priority level. Please enter a value between [0-3].");
					}
				}
				else
				{
					printf(
							"Unrecognized ID for Process ID parameter. Please enter an ID between [1-9].");
				}
			}
			else
			{
				printf(
						"Wrong format for set priority command.  Please try again.");
			}
			release_memory_block(oMsg);
		}
	}
}
void user_process_a()
{
	messageEnvelope *oMsg = NULL;
	int nRecType;

	// Register with the KCD to handle commands starting with '%Z'
	oMsg = (messageEnvelope*) request_memory_block();
	oMsg->data[0] = 'Z';
	oMsg->message_type = REGISTER_CMD;
	send_message(SYS_KEYBOARD_DECODER_PID, oMsg);

	while (1)
	{
		//Receive message.
		oMsg = receive_message(NULL);
		//Save message contents.
		nRecType = oMsg->message_type;

		//if message contains %Z command then exit
		if ((nRecType == CMD_REQUEST) && (strncmp((char *) oMsg->data, "Z", 2)
				== 0))
		{
			release_memory_block(oMsg);
			break;
		}
		else
		{
			release_memory_block(oMsg);
		}
	}

	BYTE num = 0;

	while (1)
	{
		oMsg = (messageEnvelope*) request_memory_block();
		oMsg->message_type = COUNT_REPORT;
		//set msg_data[0] field to num
		oMsg->data[0] = num;
		//send the message to process B
		send_message(USER_PROC_B_PID, oMsg);

		num += 1;
		release_processor();
	}
}

void user_process_b()
{
	messageEnvelope *oMsg = NULL;

	while (1)
	{
		//receive message
		oMsg = receive_message(NULL);
		//send message to process c
		send_message(USER_PROC_C_PID, oMsg);
	}
}

void user_process_c()
{
	messageEnvelope *oMsg = NULL;

	//perform any needed initialization and create a local message queue
	msg_queue oMailBox;
	init_rtx_queue(&oMailBox);

	while (1)
	{
		if (oMailBox.oHead == NULL)
			//Receive message
			oMsg = receive_message(NULL);
		else
			oMsg = (messageEnvelope *) pop_rtx_queue(&oMailBox);

		if (oMsg->message_type == COUNT_REPORT)
		{
			int remainder = ((int) oMsg->data[0]) % 20;

			if (remainder == 0)
			{
				//Send "Process C" to CRT display using oMsg
				sprintf((char *) oMsg->data, "%s", "Process C\n\r");
				send_message(SYS_CRT_DISPLAY_PID, oMsg);

				//Hibernate for 10 seconds:

				oMsg = (messageEnvelope *) request_memory_block();

				//Request a delayed_send for 10 sec delay with msg_type=wakeup10 using oHibernateMsg
				oMsg->message_type = WAKEUP_10;
				delayed_send(USER_PROC_C_PID, oMsg, 10000);

				while (1)
				{
					//receive a message //block and let other processes execute
					oMsg = receive_message(NULL);

					if (oMsg->message_type == WAKEUP_10)
						break;
					else
						push_rtx_queue(&oMailBox, oMsg);
				}
			}
		}
		release_memory_block(oMsg);
		release_processor();
	}
}

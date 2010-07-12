#ifndef _QUEUE_H_
#define _QUEUE_H_

char pop_from_queue(volatile char ** firstPtr, volatile char ** lastPtr, char * queueStart, int queueSize );
VOID insert_to_queue( char toInsert,  volatile char ** firstPtr,  volatile char ** lastPtr, char * queueStart, int queueSize );

#endif

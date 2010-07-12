#include "dbug.h"
#include "queue.h"
// The array
// [ .... f xxxxx l ......]
// . is empty
// f is firstPtr
// l is lastPtr
// x is data

//inserts at lastPtr. modifies lastPtr. If queue is full, it blocks
VOID insert_to_queue( char toInsert, volatile char ** firstPtr,  volatile char ** lastPtr, char * queueStart, int queueSize )
{
	//some asserts go here

	//modify lastPtr as late as possible.
	char * newLastPtr = (char *) *lastPtr;
	char * queueEnd = queueStart + queueSize - 1; //points to the last available slot in array
	if( newLastPtr >= queueEnd )
	{
		newLastPtr = queueStart;
	}
	else
	{
		newLastPtr++;
	}
	
	while( *firstPtr == newLastPtr );
	**lastPtr = toInsert;
	*lastPtr = newLastPtr;
	
	return;
}

//Pop call. If nothing on queue, it blocks. Returns the char to character. Modifies firstPtr
char pop_from_queue(volatile  char ** firstPtr,volatile  char ** lastPtr, char * queueStart, int queueSize )
{
	//some asserts go here
	if( firstPtr == NULL || *firstPtr == NULL || lastPtr == NULL || *lastPtr == NULL || queueStart == NULL || queueSize == 0 )
	{
		return NULL;
	}
	
	char toRet;
	while( *firstPtr == *lastPtr );
	toRet = **firstPtr;
	char * queueEnd = queueStart + queueSize - 1; //points to the last available slot in array
	if( *firstPtr >= queueEnd )
	{
		*firstPtr = queueStart;
	}
	else
	{
		*firstPtr = *firstPtr + 1;
	}
	return toRet;
}
void print_queue( char * queueStart, char * queueEnd, char * queueBeginning, int queueSize )
{
	char * queueEnd2 = queueStart + queueSize - 1; //points to the last available slot in array
	//printf("queue contents...");
	while( queueStart != queueEnd )
	{
	//	printf( "%c", *queueStart );
		queueStart++;
		if( queueStart > queueEnd2 )
		{
			queueStart = queueBeginning;
		}
	}
//	printf(" ");
	
}
void print_array( char * queueStart, int queueSize )
{
	int i;
	char * queuePtr = queueStart;
	//printf("Printing...\n");
	//nprintf("Chars...");
	for( i = 0; i < queueSize; i++ )
	{
		//printf( "%c", *queuePtr );
		queuePtr++;
	}
	//printf("\n");
	/*printf("Ints.....");
	queuePtr = queueStart;
	for( i = 0; i < queueSize; i++ )
	{
		printf( "%i", *queuePtr );
		queuePtr++;
	}
	//printf("\n");*/
}

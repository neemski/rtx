/*
 * memory.c
 *
 *  Created on: 2010-05-30
 *      Author: mzakharo
 */

#include "dbug.h"
#include "kernel_globals.h"
#include "rtx_api.h"
#define MEM_BLOCK_STRUCT_SIZE 	(sizeof(memory_block) - sizeof(void *)) //we need to subtract our user_memory pointer - it is just a placeholder
#define MIN_REQUEST_SIZE 		8 // 8 byte aligned memory
#define MIN_BLOCK_SIZE 			(MEM_BLOCK_STRUCT_SIZE + MIN_REQUEST_SIZE)
#define COUNT_MEM_BLKS 0
#define COUNT_MEM_BLK_SIZES 1


#undef printf
#define printf(format, ...){rtx_dbug_outs((CHAR *) "rtx_kern: ");\
							printf(format, ## __VA_ARGS__); };

void init_memory(void * start_addr, void * end_addr)
{
	mem_list_head = (memory_block*) start_addr;
	mem_list_head->next = NULL;
	mem_list_head->size = (UINT32) end_addr - (UINT32) start_addr;
	printf("Memory of %d KB initialised", (int) mem_list_head->size / 1024);
}
void* kernel_malloc(size_t req_size)
{
	memory_block * head;
	memory_block * previous = NULL;
	UINT32 total_req_size;
	memory_block* request;
	head = mem_list_head;

	if (req_size < MIN_REQUEST_SIZE)
		total_req_size = MIN_REQUEST_SIZE + MEM_BLOCK_STRUCT_SIZE;
	else
		total_req_size = req_size + MEM_BLOCK_STRUCT_SIZE;

	while (head != NULL)
	{
		//we found a free block thats big enough to be split
		if (head->size >= total_req_size + MIN_BLOCK_SIZE)
		{

			//reduce the size of the current block
			head->size -= total_req_size;
			request = (memory_block *) ((UINT32) head + head->size);
			request->size = total_req_size;
			break;
		}
		//we found a perfectly sized free block
		else if (head->size == total_req_size)
		{
			request = head;
			previous->next = head->next; //pop from the free list
			break;
		}
		previous = head;
		head = head->next;
	}
	if (head == NULL)
		return NULL;
	return &(request->user_memory_placeholder);
}
void kernel_free(void* ptr)
{
	memory_block * next;
	memory_block * previous = NULL;
	memory_block * real_ptr;
	if (ptr == NULL)
		return;

	next = mem_list_head;
	real_ptr = container_of(ptr, struct memory_block, user_memory_placeholder);
	while (next != NULL)
	{
		if (next > real_ptr)
			break;
		previous = next;
		next = next->next;
	}
	//insert the block into the free list
	previous->next = real_ptr;
	real_ptr->next = next;

	// De-fragment memory if there are nearby blocks in the free list
	if ((next != NULL) && (((memory_block *) ((UINT32) real_ptr
			+ real_ptr->size)) == next))
	{
		real_ptr->size += next->size;
		real_ptr->next = next->next;
	}
	if (((memory_block *) ((UINT32) previous + previous->size)) == real_ptr)
	{
		previous->size += real_ptr->size;
		previous->next = real_ptr->next;
	}
}

int count_memory_blocks(memory_block * head)
{
	if (head == NULL)
		return 0;
	return 1 + count_memory_blocks(head->next);
}
int count_memory_block_sizes(memory_block * head)
{
	if (head == NULL)
		return 0;
	return head->size + count_memory_block_sizes(head->next);
}

int kernel_count_memory(int nType)
{
	if(nType == COUNT_MEM_BLKS)
	{
		return count_memory_blocks(mem_list_head);
	}
	else if(nType == COUNT_MEM_BLK_SIZES)
	{
		return count_memory_block_sizes(mem_list_head);
	}
    return -1;
}

#ifdef _DEBUG_MEM_

void print_mem(memory_block * mem)
{
	if (mem == NULL)
		return;

	printf("mem block at addr 0x%x, has size of 0x%x", (int) mem,
			(int) mem->size);
	print_mem(mem->oNext);
}

void test_mem_size()
{
	memory_block *mem = mem_list_head;
	int nSize = mem->size;
	//Populate 50% of our memory with integers.
	int nPopulateSize = (nSize / 2);
	void * testPtr = malloc(nPopulateSize);

	if (malloc(nPopulateSize + 10) == NULL)
	{
		free(testPtr);
		if (mem_list_head->size == nSize)
		{
			printf("Out of memory test passes");
		}
		else
		{
			printf("Out of memory test fails: free did not free");
		}
	}
	else
	{
		free(testPtr);
		printf("Out of memory test fails");
	}

}

void test_mem_perfect_size()
{
	void *testPtr32;
	void *testPtr40;
	testPtr32 = malloc(32);
	malloc(32);
	testPtr40 = malloc(40);

	while (malloc(32) != NULL)
		;

	free(testPtr40);
	free(testPtr32);
	if (mem_list_head->size < 40)
	{
		if (mem_list_head->oNext != NULL)
		{
			if (mem_list_head->oNext->size == 48)
			{
				if (mem_list_head->oNext->oNext->size == 40)
				{
					if (malloc(40) != NULL)
					{
						if (mem_list_head != NULL)
						{
							if (mem_list_head->oNext != NULL)
							{
								if (mem_list_head->oNext->size == 40)
								{
									printf(
											"Perfect memory size block test passes");
									print_mem(mem_list_head);
								}
								else
									printf(
											"Perfect memory size block test fails because last block is of wrong size (not equal to 40)");
							}
							else
								printf(
										"Perfect memory size block test fails because last block is missing");
						}
						else
							printf(
									"Perfect memory size block test fails because memory head is null");
					}
					else
						printf(
								"Perfect memory size block test fails because malloc(40) returned NULL");
				}
				else
					printf(
							"Perfect memory size block test fails because the size 40 block does not point to NULL");
			}
			else
				printf(
						"Perfect memory size block test fails because mem_list_head->next->size does not equal 48");
		}
		else
			printf(
					"Perfect memory size block test fails because free(40) did not work");
	}
	else
		printf(
				"Perfect memory size block test fails because mem_list_head is greater than 40");
}
#endif

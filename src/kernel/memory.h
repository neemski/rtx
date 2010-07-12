/*
 * memory.h
 *
 *  Created on: 2010-05-30
 *      Author: mzakharo
 */

#ifndef MEMORY_H_
#define MEMORY_H_
#include "rtx_inc.h"
#include <stddef.h>

typedef struct memory_block
{
	struct memory_block *next;
	UINT32 size;
	void * user_memory_placeholder; //placeholder

} memory_block;

void * kernel_malloc(size_t size);
void kernel_free(void* ptr);

void init_memory(void * start_addr, void * end_addr);
void print_mem(memory_block * mem);
void test_mem_size();
void test_mem_perfect_size();
int count_memory_blocks(memory_block * head);
int count_memory_block_sizes(memory_block * head);
int kernel_count_memory(int);

#endif /* MEMORY_H_ */

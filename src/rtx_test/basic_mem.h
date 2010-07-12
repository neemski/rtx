/*
 * basic_mem.h
 *
 *  Created on: 2010-07-09
 *      Author: Michael Bieniek
 */

#ifndef BASIC_MEM_H_
#define BASIC_MEM_H_


/* This test case performs some simple release and request memory block calls.
 * The first portion of the test checks to see if memory blocks can be requested and
 * released without issues.
 * The second portion of the test requests 30 memory blocks and subsequently releases them.
 * If the process is pre-empted then the test fails because that means that 30 memory blocks
 * are not available (as required by specifications).
 */
void req_rel_mem_blk_test1();
void req_rel_mem_blk_test2();

#endif /* BASIC_MEM_H_ */

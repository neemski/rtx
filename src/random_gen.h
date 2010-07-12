/*
 * random_gen.h
 *
 *  Created on: Jun 24, 2010
 *      Author: Vlad
 */

#ifndef RANDOM_GEN_H_
#define RANDOM_GEN_H_

void init_twister(void);
void sgenrand(unsigned long int rng_num, unsigned long seed);
unsigned long genrand(unsigned long int rng_num);

#endif /* RANDOM_GEN_H_ */

//#include <assert.h>

/* mt19937int.c */
/*
static void init_twister(void);
static void sgenrand(unsigned long int rng_num, unsigned long seed);
static unsigned long genrand(unsigned long int rng_num);*/
#include "random_gen.h"

/* Make the size dynamic eventually. */
#define	MAX_DESC	100


/* A C-program for MT19937: Integer     version                   */
/*  genrand() generates one pseudorandom unsigned integer (32bit) */
/* which is uniformly distributed among 0 to 2^32-1  for each     */
/* call. sgenrand(seed) set initial values to the working area    */
/* of 624 words. Before genrand(), sgenrand(seed) must be         */
/* called once. (seed is any 32-bit integer except for 0).        */
/*   Coded by Takuji Nishimura, considering the suggestions by    */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.              */

/* This library is free software; you can redistribute it and/or   */
/* modify it under the terms of the GNU Library General Public     */
/* License as published by the Free Software Foundation; either    */
/* version 2 of the License, or (at your option) any later         */
/* version.                                                        */
/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of  */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            */
/* See the GNU Library General Public License for more details.    */
/* You should have received a copy of the GNU Library General      */
/* Public License along with this library; if not, write to the    */
/* Free Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   */
/* 02111-1307  USA                                                 */

/* Copyright (C) 1997 Makoto Matsumoto and Takuji Nishimura.       */
/* Any feedback is very welcome. For any question, comments,       */
/* see http://www.math.keio.ac.jp/matumoto/emt.html or email       */
/* matumoto@math.keio.ac.jp                                        */


/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfU   /* constant vector a */
#define UPPER_MASK 0x80000000U /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffU /* least significant r bits */

/* Tempering parameters */
#define TEMPERING_MASK_B 0x9d2c5680U
#define TEMPERING_MASK_C 0xefc60000U
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)


#define	NUM_RNG	100		/* JTD */


static unsigned long mt[NUM_RNG][N]; /* the array for the state vector  */
/* was static int mti=N+1;  mti[x]==N+1 means mt[x][N] is not initialized */
static int mti[NUM_RNG]; /* mti[x]==N+1 means mt[x][N] is not initialized */


/*+ Call this before sgenrand. +*/
void init_twister(void)
{
    int		i;

    for (i = 0; i < NUM_RNG; i++) {
	mti[i] = N+1;	/* mti[x]==N+1 means mt[x][N] is not initialized */
    }
}


/*+ initializing the array with a NONZERO seed. +*/
void sgenrand(unsigned long int rng_num, unsigned long seed)
{
    /* setting initial seeds to mt[x][N] using         */
    /* the generator Line 25 of Table 1 in          */
    /* [KNUTH 1981, The Art of Computer Programming */
    /*    Vol. 2 (2nd Ed.), pp102]                  */
    mt[rng_num][0]= seed & 0xffffffffU;
    for (mti[rng_num]=1; mti[rng_num]<N; mti[rng_num]++)
        mt[rng_num][mti[rng_num]] = (69069 * mt[rng_num][mti[rng_num]-1]) &
	    0xffffffffU;
}


/*+ Make a random number. +*/
unsigned long genrand(unsigned long int rng_num)
{
    unsigned long y;

    static int	first = 1;
/* was static unsigned long mag01[2]={0x0, MATRIX_A}; */
    static unsigned long mag01[NUM_RNG][2];
    /* mag01[rng_num][x] = x * MATRIX_A  for x=0,1 */

    //assert(rng_num < NUM_RNG);
    if( rng_num >= NUM_RNG )
    {
    	//this shouldn't happen.
    	return -1;
    }


	/* Init mag01 */
    if (first == 1) {
	int	i;

	for (i = 0; i < NUM_RNG; i++) {
	    mag01[i][0] = 0x0;
	    mag01[i][1] = MATRIX_A;
	}

	first = 0;
    }

    if (mti[rng_num] >= N) { /* generate N words at one time */
        int kk;

        if (mti[rng_num] == N+1) /* if sgenrand() has not been called, */
            sgenrand(rng_num, 4357);	 /* a default initial seed is used   */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[rng_num][kk]&UPPER_MASK)|(mt[rng_num][kk+1]&LOWER_MASK);
            mt[rng_num][kk] = mt[rng_num][kk+M] ^
		(y >> 1) ^ mag01[rng_num][y & 0x1];
        }
        for (;kk<N-1;kk++) {
            y = (mt[rng_num][kk]&UPPER_MASK)|(mt[rng_num][kk+1]&LOWER_MASK);
            mt[rng_num][kk] = mt[rng_num][kk+(M-N)] ^
		(y >> 1) ^ mag01[rng_num][y & 0x1];
        }
        y = (mt[rng_num][N-1]&UPPER_MASK)|(mt[rng_num][0]&LOWER_MASK);
        mt[rng_num][N-1] = mt[rng_num][M-1] ^
	    (y >> 1) ^ mag01[rng_num][y & 0x1];

        mti[rng_num] = 0;
    }

    y = mt[rng_num][mti[rng_num]++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);

    return y;
}

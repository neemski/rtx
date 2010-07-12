/*
 * process.h
 *
 *  Created on: 2010-05-28
 *      Author: mzakharo
 */

#ifndef PROCESS_H_
#define PROCESS_H_
#include "rtx_inc.h"

enum PROCTYPE {
	SYS_PROC,
	USER_PROC
};
#define I_PRIORITY -1

void init_processes();
#endif /* PROCESS_H_ */

/*
 * abc.h
 *
 *  Created on: 15-Feb-2023
 *      Author: Jitu
 */

//#ifndef INC_ABC_H_
#define INC_ABC_H_

typedef unsigned char                uint8_t;
typedef unsigned short int        uint16_t;
#ifndef __uint32_t_defined
typedef unsigned long int                uint32_t;
typedef  uint8_t bool_t;
# define __uint32_t_defined
#endif
#if __WORDSIZE == 64
//typedef unsigned long int        uint64_t;
#else
__extension__
typedef unsigned long long int        uint64_t;

#define true	1
#define false	0



#endif /* INC_ABC_H_ */

/*
 * vm.h
 *
 *  Created on: Jul 31, 2010
 *      Author: ruslan
 */

#ifndef VM_H_
#define VM_H_

#include "types.h"

enum VmInsruction {
	VMI_MOV,
	VMI_ADD,
	VMI_DIV,
	VMI_SUB,
	VMI_MUL,
	VMI_JE,
	VMI_JZ,
	VMI_CMP,
	VMI_CALL,
	VMI_PRN,
	VMI_HALT,
	VMI_STW,
	VMI_STB,
	VMI_LDW,
	VMI_LDB
};

enum VmRegister {
	R01 = 0,
	R02,
	R03,
	R04,
	R05,
	R06,
	R07,
	R08,
	R09,
	R10,
	R11,
	R12,
	R13,
	R14,
	R15
};

extern void execute(byte instructions[]);


#endif /* VM_H_ */

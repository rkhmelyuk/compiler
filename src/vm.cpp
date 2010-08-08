/*
 * vm.cpp
 *
 *  Created on: Jul 31, 2010
 *      Author: ruslan
 */

#include "vm.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

static int reg[15];
static byte memory[1024];

void execute(byte instructions[]) {
	memset(memory, 0, 1024);

	int i = 0;
	do {
		byte op = instructions[i++];
		if (op == VMI_ADD) {
			byte op1 = instructions[i++];
			byte op2 = instructions[i++];
			reg[op1] = reg[op1] + reg[op2];
		}
		else if (op == VMI_SUB) {
			byte op1 = instructions[i++];
			byte op2 = instructions[i++];
			reg[op1] = reg[op1] - reg[op2];
		}
		else if (op == VMI_MOV) {
			byte op1 = instructions[i++];
			reg[op1] = (instructions[i] << 24) + (instructions[i+1] << 16) + (instructions[i+2] << 8) + instructions[i+3];
			i += 4;
		}
		else if (op == VMI_LDW) {
			byte op1 = instructions[i++];
			int address = instructions[i++];
			reg[op1] = (memory[address] << 24) + (memory[address+1] << 16) + (memory[address+2] << 8) + memory[address+3];
		}
		else if (op == VMI_STW) {
			byte op1 = instructions[i++];
			int address = instructions[i++];
			memory[address] = (reg[op1] >> 24) & 0xFF;
			memory[address+1] = (reg[op1] >> 16) & 0xFF;
			memory[address+2] = (reg[op1] >> 8) & 0xFF;
			memory[address+3] = (reg[op1]) & 0xFF;
		}
		else if (op == VMI_PRN) {
			byte op1 = instructions[i++];
			printf("%d\n", reg[op1]);
		}
		else if (op == VMI_HALT) {
			break;
		}
	}
	while (true);
}

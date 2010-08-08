/*
 * vmcompile.cpp
 *
 *  Created on: Jul 31, 2010
 *      Author: ruslan
 */

#include "stdlib.h"
#include "stdio.h"

#include "vmcompile.h"
#include "types.h"

void _compile(AstNode* ast);
byte compilePrint(AstNode* ast);
byte compileCalc(AstNode* ast);
byte nextReg();
void freeReg();

static byte program[256];
static int index = 0;

static byte currentReg = 255;

byte* compile(AstNode* ast) {

	_compile(ast);

	program[index] = VMI_HALT;
	return program;
}

void _compile(AstNode* ast) {
	if (ast != null) {
		switch (ast->type) {
			case SEQUENCE:
				_compile(ast->left);
				_compile(ast->right);
				break;
			case PRINT:
				compilePrint(ast);
				break;
		}
	}
}

byte compilePrint(AstNode* ast) {
	byte reg = compileCalc(ast->left);
	program[index++] = VMI_PRN;
	program[index++] = reg;
	freeReg();
}

byte compileCalc(AstNode* ast) {
	if (ast != null) {
		int32  value;
		byte reg, reg2;
		switch (ast->type) {
			case ADD:
				reg = compileCalc(ast->left);
				reg2 = compileCalc(ast->right);
				program[index++] = VMI_ADD;
				program[index++] = reg;
				program[index++] = reg2;
				freeReg();
				return reg;
			case NUMBER:
				program[index++] = VMI_MOV;
				reg = nextReg();
				program[index++] = reg;
				value = (int32) ast->value;
				program[index++] = (value >> 24) & 0xFF;
				program[index++] = (value >> 16) & 0xFF;
				program[index++] = (value >> 8) & 0xFF;
				program[index++] = (value) & 0xFF;
				return reg;
			default:
				printf("Unexpected node type %d", ast->type);
				exit(1);
		}
	}
}

byte nextReg() {
	return ++currentReg;
}

void freeReg() {
	if (currentReg != -1) {
		currentReg--;
	}
}

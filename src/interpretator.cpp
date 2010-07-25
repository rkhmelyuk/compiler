/*
 * interpretator.cpp
 *
 *  Created on: Jul 25, 2010
 *      Author: ruslan
 */

#include <list>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "interpretator.h"
#include "synparser.h"

using namespace std;

struct VarObject {
	char *name;
	int32 value;
};

static list<VarObject*> vars;

void define(AstNode* ast);
void assign(AstNode* ast);
void ifThenElse(AstNode* ast);
void calculate(AstNode* ast);
int calc(AstNode* ast);
bool compare(AstNode* ast);

VarObject* findVar(char* name);

void interpretate(AstNode* ast) {
	if (ast != null) {
		switch (ast->type) {
			case SEQUENCE:
				interpretate(ast->left);
				interpretate(ast->right);
				break;
			case DEF:
				define(ast);
				break;
			case CALC:
				calculate(ast);
				break;
			case ASSIGN:
				assign(ast);
				break;
			case IF:
				ifThenElse(ast);
				break;
		}
	}
}

void define(AstNode* ast) {
	VarObject* var = new VarObject;

	var->name = (char*) ast->left->value;
	var->value = calc(ast->right);
	vars.push_back(var);
}

void assign(AstNode* ast) {
	VarObject* var = findVar((char*) ast->left->value);
	var->value = calc(ast->right);
}

void ifThenElse(AstNode* ast) {
	bool result = compare(ast->left);
	if (result) {
		interpretate(ast->right->left);
	}
	else if (ast->right->right != null){
		interpretate(ast->right->right);
	}
}

void calculate(AstNode* ast) {
	printf("%d\n", calc(ast->left));
}

int32  calc(AstNode* ast) {
	if (ast != null) {

		VarObject* var = null;
		int32  left, right;
		switch (ast->type) {
			case ADD:
				left = calc(ast->left);
				right = calc(ast->right);
				return left + right;
			case MUL:
				left = calc(ast->left);
				right = calc(ast->right);
				return left * right;
			case DIV:
				left = calc(ast->left);
				right = calc(ast->right);
				return left / right;
			case SUB:
				left = calc(ast->left);
				right = calc(ast->right);
				return left - right;
			case NUMBER:
				return (int32) ast->value;
			case IDENT:
				var = findVar((char*) ast->value);
				return var->value;
			default:
				printf("Unexpected node type %d", ast->type);
				exit(1);
		}
	}
}

bool compare(AstNode* ast) {
	if (ast != null) {
		int32 left = calc(ast->left);
		int32 right = calc(ast->right);

		switch (ast->type) {
			case EQUAL:
				return left == right;
			case NOT_EQUAL:
				return left != right;
			case LESS:
				return left < right;
			case LESS_EQUAL:
				return left <= right;
			case GREATER:
				return left > right;
			case GREATER_EQUAL:
				return left >= right;
			default:
				printf("Unexpected comparison type %d", ast->type);
				exit(1);
		}
	}
	return false;
}

VarObject* findVar(char* name) {
	for (list<VarObject*>::iterator i = vars.begin(); i != vars.end(); i++) {
		VarObject *obj = *i;

		if (strcmp(obj->name, name) == 0) {
			return obj;
		}
	}

	printf("Unknown variable %s", name);
	exit(1);

	return null;
}

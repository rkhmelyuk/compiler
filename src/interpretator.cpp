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
	int value;
};

static list<VarObject*> vars;

void define(AstNode* ast);
void calculate(AstNode* ast);
int calc(AstNode* ast);

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
		}
	}
}

void define(AstNode* ast) {
	VarObject* var = new VarObject;

	var->name = (char*) ast->left->value;
	if (ast->right->type == NUMBER) {
		var->value = atoi((char*) ast->right->value);
	}
	else if (ast->right->type == IDENT) {
		VarObject* refVar = findVar((char*) ast->right->value);
		var->value = refVar->value;
	}
	vars.push_back(var);
}

void calculate(AstNode* ast) {
	printf("%d", calc(ast->left));
}

int calc(AstNode* ast) {
	if (ast != null) {

		VarObject* var = null;
		int left, right;
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
				return atoi((char*) ast->value);
			case IDENT:
				var = findVar((char*) ast->value);
				return var->value;
			default:
				printf("Unexpected node type %d", ast->type);
				exit(1);
		}
	}
}

VarObject* findVar(char* name) {
	for (list<VarObject*>::iterator i = vars.begin(); i != vars.end(); i++) {
		VarObject *obj = *i;

		if (strcmp(obj->name, name) == 0) {
			return obj;
		}
	}
	return null;
}

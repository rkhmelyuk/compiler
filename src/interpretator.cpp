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
#include "ast.h"

using namespace std;

struct VarObject {
	char *name;
	int32 value;
};

struct FuncObject {
	char *name;
	AstNode *value;
};


static list<VarObject*> vars;
static list<FuncObject*> funcs;

int _interpretate(AstNode* ast);
void define(AstNode* ast);
void assign(AstNode* ast);
int ifThenElse(AstNode* ast);
int whileLoop(AstNode* ast);
void print(AstNode* ast);
int calc(AstNode* ast);
bool compare(AstNode* ast);
void defineFunc(AstNode* ast);
void callFunc(AstNode* ast);

VarObject* findVar(char* name);

void interpretate(AstNode* ast) {
	_interpretate(ast);
}

int _interpretate(AstNode* ast) {
	if (ast != null) {
		int result = 0;
		switch (ast->type) {
			case SEQUENCE:
				result = _interpretate(ast->left);
				if (result != 0) return result;
				return _interpretate(ast->right);
			case DEF:
				define(ast);
				break;
			case FUNC:
				defineFunc(ast);
				break;
			case FUNC_CALL:
				callFunc(ast);
				break;
			case PRINT:
				print(ast);
				break;
			case ASSIGN:
				assign(ast);
				break;
			case IF:
				return ifThenElse(ast);
			case WHILE:
				return whileLoop(ast);
			case BREAK:
				return -1;
			case CONTINUE:
				return -2;
		}
	}
	return 0;
}

void defineVar(char* name, int value) {
	VarObject* var = new VarObject;

	var->name = name;
	var->value = value;
	vars.push_back(var);
}

void define(AstNode* ast) {
	DefObject *object = (DefObject*) ast->left->value;
	defineVar(object->name, calc(ast->right));
}

void defineFunc(AstNode* ast) {
	FuncObject* func = new FuncObject;

	DefObject *object = (DefObject*) ast->value;
	func->name = object->name;
	func->value = ast;
	funcs.push_back(func);
}

void callFunc(AstNode* ast) {
	FuncObject* func = null;
	for (list<FuncObject*>::reverse_iterator i = funcs.rbegin(); i != funcs.rend(); i++) {
		FuncObject *obj = *i;

		if (strcmp(obj->name, (char*) ast->value) == 0) {
			func = obj;
			break;
		}
	}

	if (func == null) {
		printf("Unknown variable %s", (char*) ast->value);
		exit(1);
	}

	AstNode* node = func->value->left;
	AstNode* valNode = ast->left;
	while (node != null && node->left != null && valNode != null && valNode->left != null) {
		DefObject *object = (DefObject*) node->left->left->value;
		defineVar(object->name, calc(valNode->left));

		node = node->right;
		valNode = valNode->right;
	}

	_interpretate(func->value->right);
}

void assign(AstNode* ast) {
	VarObject* var = findVar((char*) ast->left->value);
	var->value = calc(ast->right);
}

int ifThenElse(AstNode* ast) {
	bool result = compare(ast->left);
	if (result) {
		return _interpretate(ast->right->left);
	}
	else if (ast->right->right != null){
		return _interpretate(ast->right->right);
	}
	return 0;
}

int whileLoop(AstNode* ast) {
	bool result = false;
	while ((result = compare(ast->left)) == true) {
		int val = _interpretate(ast->right);
		if (val == -1) {
			break;
		}
	}
	return 0;
}

void print(AstNode* ast) {
	if (ast->left->type == STRING) {
		printf("%s\n", (char*) ast->left->value);
	}
	else {
		printf("%d\n", calc(ast->left));
	}
}

int32 calc(AstNode* ast) {
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
	for (list<VarObject*>::reverse_iterator i = vars.rbegin(); i != vars.rend(); i++) {
		VarObject *obj = *i;

		if (strcmp(obj->name, name) == 0) {
			return obj;
		}
	}

	printf("Unknown variable %s", name);
	exit(1);

	return null;
}

/*
 * ast.cpp
 *
 *  Created on: Jul 25, 2010
 *      Author: ruslan
 */

#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "types.h"

using namespace std;

AstNode* stringToNumbers(AstNode* ast);
AstNode* foldConstants(AstNode* ast);

void pushDefContext(char* name);
void popDefContext();

void addDefObject(AstNode* node, DefObject *object);
DefObject* getDefObject(AstNode *node, ObjectType type);
void check(AstNode* ast);
void checkFuncArgs(DefObject *func, AstNode* ast);

static list<DefContext*> *contexts;
static bool hasSemanticError;

void semanticError(AstNode *node, char *error) {
	if (node != null) {
		printf("Error: %s in file %s at position [%d, %d]\n",
				error, node->location->file,
				node->location->row,
				node->location->column);
	}

	hasSemanticError = true;
}


AstNode* semanticCheck(AstNode* ast) {
	contexts = new list<DefContext*>();
	pushDefContext("global");

	check(ast);

	popDefContext();

	delete contexts;
	contexts = null;

	if (hasSemanticError) {
		printf("COMPILATION FAILED!\n");
		return null;
	}
	return ast;
}

void check(AstNode* ast) {
	if (ast == null) return;

	switch (ast->type) {
			case DEF:
				check(ast->right);
				addDefObject(ast, (DefObject *) ast->left->value);
				break;
			case FUNC:
				addDefObject(ast, (DefObject *) ast->value);
				pushDefContext("func");
				check(ast->left);
				check(ast->right);
				popDefContext();
				break;
			case THEN:
			case ELSE:
			case WHILE:
				pushDefContext("$$");
				check(ast->left);
				check(ast->right);
				popDefContext();
				break;
			case FUNC_CALL:
				checkFuncArgs(getDefObject(ast, FUNCTION), ast->left);
				check(ast->left);
				check(ast->right);
				break;
			case IDENT:
				getDefObject(ast, VARIABLE);
				check(ast->left);
				check(ast->right);
				break;
			default:
				check(ast->left);
				check(ast->right);
		}
}

void checkFuncArgs(DefObject *func, AstNode* realArgs) {
	int funcArgs = 0;
	AstNode *args = func->node->left;
	while (args != null && args->type == SEQUENCE) {
		funcArgs++;
		args = args->right;
	}

	int realArgsNum = 0;

	args = realArgs;
	while (args != null && args->type == SEQUENCE) {
		realArgsNum++;
		args = args->right;
	}

	if (funcArgs != realArgsNum) {
		semanticError(realArgs, "Wrong arguments list");
	}
}

void addDefObject(AstNode *node, DefObject *object) {
	DefContext* context = contexts->back();

	for (list<DefObject*>::iterator iter = context->objects->begin(); iter != context->objects->end(); iter++) {
		DefObject *obj = *iter;
		if (object->type == obj->type && strcmp(object->name, obj->name) == 0) {
			// object is already defined
			if (object->type == FUNCTION) {
				semanticError(node, "Function with such name is already defined");
			}
			else if (object->type == VARIABLE) {
				semanticError(node, "Variable with such name is already defined");
			}
		}
	}

	object->node = node;

	context->objects->push_back(object);
}

DefObject* getDefObject(AstNode *node, ObjectType type) {
	char *name = (char*) node->value;
	for (list<DefContext*>::reverse_iterator citer = contexts->rbegin(); citer != contexts->rend(); ++citer) {
		DefContext* context = *citer;
		for (list<DefObject*>::iterator iter = context->objects->begin(); iter != context->objects->end(); iter++) {
			DefObject *obj = *iter;
			if (type == obj->type && strcmp(name, obj->name) == 0) {
				// object is already defined
				return obj;
			}
		}
	}
	if (type == FUNCTION) {
		char *message = (char*) malloc(30 + strlen(name));
		sprintf(message, "Unknown function found '%s'", name);
		semanticError(node, message);
	}
	else if (type == VARIABLE) {
		char *message = (char*) malloc(30 + strlen(name));
		sprintf(message, "Unknown variable found '%s'", name);
		semanticError(node, message);
	}

	return null;
}


// -------------------------------------------------------

AstNode* optimize(AstNode* ast) {
	ast = stringToNumbers(ast);
	ast = foldConstants(ast);

	return ast;
}

void pushDefContext(char* name) {
	DefContext *context = new DefContext;
	context->objects = new list<DefObject*>;
	context->name = name;
	contexts->push_back(context);
}

void popDefContext() {
	DefContext *context = contexts->back();
	contexts->pop_back();

	delete context->objects;
	delete context;
}

AstNode* stringToNumbers(AstNode* ast) {
	if (ast == null) return null;

	if (ast->type == NUMBER) {
		int value = atoi((char*)ast->value);
		ast->value = (void*) value;
	}
	else {
		ast->left = stringToNumbers(ast->left);
		ast->right = stringToNumbers(ast->right);
	}
	return ast;
}

AstNode* foldConstants(AstNode* ast) {
	if (ast == null) return null;

	ast->left = foldConstants(ast->left);
	ast->right= foldConstants(ast->right);

	if (ast->left != null && ast->right != null && ast->left->type == NUMBER && ast->right->type == NUMBER) {
		int32 left = (int32) ast->left->value;
		int32 right= (int32) ast->right->value;
		switch (ast->type) {
			case ADD:
				ast->value = (void*) left + right;
				break;
			case SUB:
				ast->value = (void*) left - right;
				break;
			case MUL:
				ast->value = (void*) (left * right);
				break;
			case DIV:
				ast->value = (void*) (left / right);
				break;
			default:
				return ast;
		}

		ast->type = NUMBER;

		delete ast->left;
		delete ast->right;
		ast->left = null;
		ast->right= null;
	}

	return ast;
}

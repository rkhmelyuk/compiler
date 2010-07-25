/*
 * ast.h
 *
 *  Created on: Jul 25, 2010
 *      Author: ruslan
 */

#ifndef AST_H_
#define AST_H_

#include "types.h"

enum AstNodeType {
	SEQUENCE = 1,
	ASSIGN,
	EQUAL,
	NOT_EQUAL,
	GREATER_EQUAL,
	GREATER,
	LESS_EQUAL,
	LESS,
	ADD,
	SUB,
	DIV,
	DEF,
	CALC,
	IDENT,
	NUMBER,
	MUL,
	IF,
	THEN,
	ELSE
};

struct AstNode {
	void *value;
	AstNodeType type;
	AstNode *left;
	AstNode *right;

	AstNode(): value(null), left(null), right(null){}
};

extern AstNode* optimize(AstNode* ast);

#endif /* AST_H_ */

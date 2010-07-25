/*
 * ast.h
 *
 *  Created on: Jul 25, 2010
 *      Author: ruslan
 */

#ifndef AST_H_
#define AST_H_

enum AstNodeType {
	SEQUENCE = 1,
	EQUAL,
	ADD,
	SUB,
	DIV,
	DEF,
	CALC,
	IDENT,
	NUMBER,
	MUL
};

struct AstNode {
	void *value;
	AstNodeType type;
	AstNode *left;
	AstNode *right;
};


#endif /* AST_H_ */

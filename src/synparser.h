/*
 * synparser.h
 *
 *  Created on: Jul 1, 2010
 *      Author: Ruslan Khmelyuk
 */

#ifndef SYNPARSER_H_
#define SYNPARSER_H_

#include "types.h"
#include "lexparser.h"

#define SYNTAX_ERROR 65
#define null NULL

enum ObjectType {
	VARIABLE,
	FUNCTION
};

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
	MUL,
	GROUP,
};

struct DefObject {
	char *name;
	//char *value;
	ObjectType type;
};

struct DefObjectNode {
	DefObject *obj;
	DefObjectNode *next;
};

struct DefContext {
	char *name;
	DefObjectNode *first;
	DefObjectNode *last;
};

struct AstNode {
	void *value;
	AstNodeType type;
	AstNode *left;
	AstNode *right;
};

extern AstNode* synparse(NodeList *nodes);

#endif /* SYNPARSER_H_ */

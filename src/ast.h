/*
 * ast.h
 *
 *  Created on: Jul 25, 2010
 *      Author: ruslan
 */

#ifndef AST_H_
#define AST_H_

#include <list>

#include "lexparser.h"
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
	PRINT,
	IDENT,
	NUMBER,
	MUL,
	IF,
	THEN,
	ELSE,
	WHILE,
	BREAK,
	CONTINUE,
	FUNC,
	FUNC_CALL,
	STRING,
	IMPORT
};

struct DefObject;

struct Location {
	char *file;
	int row;
	int column;

	Location(): file(null), row(0), column(0) {}
	Location(char *file, int row, int column): file(file), row(row), column(column) {}
};

struct AstNode {
	void *value;
	AstNodeType type;

	Location *location;

	AstNode *left;
	AstNode *right;

	//AstNode(): value(null), left(null), right(null) {}
	AstNode(char *filename, Node *node) {
		if (node != null) {
			location = new Location(filename, node->position->row, node->position->column);
		}
	}

	~AstNode() {
		delete location;
		delete left;
		delete right;
	}
};

enum ObjectType {
	VARIABLE,
	FUNCTION
};

enum DataType {
	TYPE_VOID,
	TYPE_INTEGER,
	TYPE_FLOAT,
	TYPE_LONG,
	TYPE_BYTE,
	TYPE_CHAR,
	TYPE_STRING
};

struct DefObject {
	char *name;
	ObjectType type;
	DataType dataType;
	char *dataTypeName;

	AstNode *node;

	DefObject(): name(null), dataTypeName(null), node(null) {}
};

struct DefContext {
	char *name;
	std::list<DefObject*> *objects;
};

extern AstNode* semanticCheck(AstNode* ast);

extern AstNode* optimize(AstNode* ast);

#endif /* AST_H_ */

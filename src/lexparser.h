/*
 * lexparser.h
 *
 *  Created on: Jun 28, 2010
 *      Author: Ruslan Khmelyuk
 */

#ifndef LEXPARSER_H_
#define LEXPARSER_H_

#include "types.h"

#define LEXER_ERROR 64

enum NodeType {
	NODE_TYPE_IDENTIFIER = 1,
	NODE_TYPE_NUMBER = 2,
	NODE_TYPE_PLUS = 3,
	NODE_TYPE_MINUS	= 4,
	NODE_TYPE_MUL = 5,
	NODE_TYPE_DIV = 6,
	NODE_TYPE_LPAREN = 7,
	NODE_TYPE_RPAREN = 8,
	NODE_TYPE_COMMENT = 9,
	NODE_TYPE_SEMICOLON = 10,
	NODE_TYPE_ASSIGN = 11,
	NODE_TYPE_DEF = 12,
	NODE_TYPE_PRINT = 13,
	NODE_TYPE_EOF = 14,
	NODE_TYPE_GREATER_EQ,
	NODE_TYPE_GREATER,
	NODE_TYPE_LESS_EQ,
	NODE_TYPE_LESS,
	NODE_TYPE_EQUAL,
	NODE_TYPE_NOT_EQUAL,
	NODE_TYPE_IF,
	NODE_TYPE_THEN,
	NODE_TYPE_ELSE,
	NODE_TYPE_LBRACE,
	NODE_TYPE_RBRACE,
	NODE_TYPE_WHILE,
	NODE_TYPE_BREAK,
	NODE_TYPE_CONTINUE,
	NODE_TYPE_FUNC,
	NODE_TYPE_COMMA,
	NODE_TYPE_STRING
};

struct Position {
	int column;
	int row;
};

struct Node {
	NodeType nodeType;
	char *value;
	Position *position;

	Node(): value(null), position(null){}
};

struct NodeList {
	Node *node;
	NodeList *prev;
	NodeList *next;

	NodeList(): node(null), prev(null), next(null){}
};

struct Sym {
	char *string;
	int pos;
	int row;
	int col;
};

/**
 * Parses the string into the list of nodes.
 */
extern NodeList* parse(char *string);

#endif /* UTIL_H_ */

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
	NODE_TYPE_LBRACE = 7,
	NODE_TYPE_RBRACE = 8,
	NODE_TYPE_COMMENT = 9,
	NODE_TYPE_SEMICOLON = 10,
	NODE_TYPE_EQUAL = 11,
	NODE_TYPE_DEF = 12
};

struct Node {
	NodeType nodeType;
	char *value;
};

struct NodeList {
	Node *node;
	NodeList *prev;
	NodeList *next;
};

struct Sym {
	char *string;
	int pos;
};

typedef Node 		Node;
typedef NodeList 	NodeList;
typedef Sym		Sym;

/**
 * Parses the string into the list of nodes.
 */
extern NodeList* parse(char *string);

#endif /* UTIL_H_ */

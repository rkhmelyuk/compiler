/*
 * synparser.h
 *
 *  Created on: Jul 1, 2010
 *      Author: Ruslan Khmelyuk
 */

#ifndef SYNPARSER_H_
#define SYNPARSER_H_

#include "types.h"
#include "ast.h"
#include "lexparser.h"

#define SYNTAX_ERROR 65

enum ObjectType {
	VARIABLE,
	FUNCTION
};

struct DefObject {
	char *name;
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

extern AstNode* synparse(NodeList *nodes);

#endif /* SYNPARSER_H_ */

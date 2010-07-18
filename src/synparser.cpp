/*
 * synparser.cpp
 *
 *  Created on: Jul 1, 2010
 *      Author: Ruslan Khmelyuk
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexparser.h"
#include "synparser.h"

NodeList* definition(NodeList *nodes);
NodeList* expression(NodeList *nodes);
NodeList* simpleTerm(NodeList *nodes);
NodeList* term(NodeList *nodes);
NodeList* factor(NodeList *nodes);
NodeList* stabilize(NodeList *nodes);
bool addDefObject(Node *currentNode, DefObject *object);
DefObject* getDefObject(Node *currentNode, char *name);
void parseNodes(NodeList *nodes);

static bool hasError;
static DefContext *globalContext;

void syntaxError(Node *node, char *error) {
	printf("Syntax error: %s at position [%d, %d]\n",
			error, node->position->row, node->position->column);
	hasError = true;
}

void unexpected(Node *node) {
	printf("Syntax error: unexpected '%s' at position [%d, %d]\n",
			node->value, node->position->row, node->position->column);
	hasError = true;
}

void synparse(NodeList *nodes) {
	hasError = false;
	globalContext = new DefContext;
	globalContext->name = "global";

    parseNodes(nodes);

    if (hasError) {
		printf("COMPILATION FAILED\n");
	}
}

void parseNodes(NodeList *nodes) {
    NodeList *list = nodes;
    while (list != NULL) {
		switch (list->node->nodeType) {
			case NODE_TYPE_DEF:
				list = definition(list);
				break;
			case NODE_TYPE_CALC:
				list = expression(list->next);
				break;
			case NODE_TYPE_EOF:
				return;
			default:
				unexpected(list->node);
				list = stabilize(list);
		}
		if (list != NULL) {
			list = list->next;
		}
	}
}

bool isStabilizeNodeType(NodeType nodeType) {
	return nodeType == NODE_TYPE_SEMICOLON || nodeType == NODE_TYPE_EOF;
}

NodeList* stabilize(NodeList* nodes) {
	do {
		nodes = nodes->next;
		if (nodes != NULL && isStabilizeNodeType(nodes->node->nodeType)) {
			break;
		}
	} while (nodes != NULL);

	return nodes;
}

NodeList* definition(NodeList *nodes) {
	nodes = nodes->next;
	if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER) {
		DefObject *object = new DefObject;
		object->name = nodes->node->value;
		object->type = VARIABLE;

		nodes = nodes->next;
		if (addDefObject(nodes->node, object)) {
			if (nodes->node->nodeType == NODE_TYPE_EQUAL) {
				nodes = nodes->next;
				if (nodes->node->nodeType == NODE_TYPE_NUMBER) {
					object->value = nodes->node->value;
					nodes = nodes->next;
				}
				else if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER) {
					printf("equal to id(%s)\n", nodes->node->value);
					object->value = nodes->node->value;
					nodes = nodes->next;
				}
				else {
					unexpected(nodes->node);
					nodes = nodes->next;
				}
			}
			if (nodes->node->nodeType != NODE_TYPE_SEMICOLON) {
				unexpected(nodes->node);
			}
		}
	}
	else {
		unexpected(nodes->node);
		nodes = nodes->next;
	}
	return nodes;
}

bool addDefObject(Node *currentNode, DefObject *object) {
	for (DefObjectNode *node = globalContext->first; node != NULL; node = node->next) {
		if (strcmp(object->name, node->obj->name) == 0) {
			// variable is already defined
			syntaxError(currentNode, "variable '%s' is already defined");
			return false;
		}
	}

	DefObjectNode *node = new DefObjectNode;
	node->obj = object;
	node->next = null;
	if (globalContext->last == null) {
		globalContext->first = node;
		globalContext->last = node;
	}
	else {
		globalContext->last->next = node;
	}
	return true;
}

DefObject* getDefObject(Node *currentNode, char *name) {
	for (DefObjectNode *node = globalContext->first; node != NULL; node = node->next) {
		if (strcmp(name, node->obj->name) == 0) {
			// variable is already defined
			return node->obj;
		}
	}
	syntaxError(currentNode, "unknown variable found '%s'");
	return null;
}

NodeList* expression(NodeList *nodes) {
	nodes = simpleTerm(nodes);

	if (nodes != NULL && nodes->node->nodeType != NODE_TYPE_SEMICOLON) {
		unexpected(nodes->node);
		nodes = stabilize(nodes);
	}

	return nodes;
}

NodeList* simpleTerm(NodeList *nodes) {
	if (nodes->node->nodeType == NODE_TYPE_PLUS
			|| nodes->node->nodeType == NODE_TYPE_MINUS) {
		nodes = nodes->next;
	}
	nodes = term(nodes);

	while (nodes->node->nodeType == NODE_TYPE_PLUS
			|| nodes->node->nodeType == NODE_TYPE_MINUS) {
		nodes = nodes->next;
		nodes = term(nodes);
	}
	return nodes;
}

NodeList* term(NodeList *nodes) {
	if (nodes->node->nodeType == NODE_TYPE_MUL
			|| nodes->node->nodeType == NODE_TYPE_DIV) {
		nodes = nodes->next;
	}
	nodes = factor(nodes);

	while (nodes->node->nodeType == NODE_TYPE_MUL
			|| nodes->node->nodeType == NODE_TYPE_DIV) {
		nodes = nodes->next;
		nodes = factor(nodes);
	}
	return nodes;
}

NodeList* factor(NodeList *nodes) {
	if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER) {
		/*DefObject *object = */getDefObject(nodes->node, nodes->node->value);
		nodes = nodes->next;
	}
	else if (nodes->node->nodeType == NODE_TYPE_NUMBER) {
		nodes = nodes->next;
	}
	else if (nodes->node->nodeType == NODE_TYPE_LBRACE) {
		nodes = nodes->next;
		do {
			nodes = simpleTerm(nodes);
			if (nodes == NULL || (nodes->node->nodeType != NODE_TYPE_RBRACE && nodes->next == NULL)) {
				syntaxError(nodes->node, "right brace is absent");
				break;
			}
		} while (nodes->node->nodeType != NODE_TYPE_RBRACE);
		nodes = nodes->next;
	}

	else {
		unexpected(nodes->node);
		nodes = stabilize(nodes);
	}
	return nodes;
}


/*
 * synparser.cpp
 *
 *  Created on: Jul 1, 2010
 *      Author: Ruslan Khmelyuk
 */

#include <stdio.h>
#include <stdlib.h>

#include "lexparser.h"
#include "synparser.h"

NodeList* definition(NodeList *nodes);
NodeList* expression(NodeList *nodes);
NodeList* simpleTerm(NodeList *nodes);
NodeList* term(NodeList *nodes);
NodeList* factor(NodeList *nodes);

static bool hasError = false;

void syntaxError(Node *node, char *error) {
	printf("Syntax error: %s at position [%d, %d]\n", error, node->position->row, node->position->column);
	hasError = true;
}

void unexpected(Node *node) {
	printf("Syntax error: unexpected '%s' at position [%d, %d]\n", node->value, node->position->row,
			node->position->column);
	hasError = true;
}

void parseNodes(NodeList *nodes)
{
    NodeList *list = nodes;
    while (list != NULL) {
		switch (list->node->nodeType) {
			case NODE_TYPE_DEF:
				list = definition(list);
				break;
			case NODE_TYPE_CALC:
				list = expression(list);
				break;
			case NODE_TYPE_EOF:
				return;
			default:
				unexpected(list->node);
		}
		if (list != NULL) {
			list = list->next;
		}
	}
}

void synparse(NodeList *nodes) {
    parseNodes(nodes);

    if (hasError) {
		printf("COMPILATION FAILED\n");
	}
}

NodeList* definition(NodeList *nodes) {
	nodes = nodes->next;
	if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER) {
		printf("found declaration of %s\n", nodes->node->value);
		nodes = nodes->next;
		if (nodes->node->nodeType == NODE_TYPE_EQUAL) {
			nodes = nodes->next;
			if (nodes->node->nodeType == NODE_TYPE_NUMBER) {
				printf("equal to number(%s)\n", nodes->node->value);
				nodes = nodes->next;
			}
			else if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER) {
				printf("equal to id(%s)\n", nodes->node->value);
				nodes = nodes->next;
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

NodeList* expression(NodeList *nodes) {
	nodes = nodes->next;
	do {
		nodes = simpleTerm(nodes);
	}
	while (nodes->node->nodeType != NODE_TYPE_SEMICOLON);

	return nodes;
}

NodeList* simpleTerm(NodeList *nodes) {
	if (nodes->node->nodeType == NODE_TYPE_MUL || nodes->node->nodeType == NODE_TYPE_DIV) {
		nodes = nodes->next;
		nodes = term(nodes);
	}
	else {
		nodes = term(nodes);
	}
	return nodes;
}

NodeList* term(NodeList *nodes) {
	if (nodes->node->nodeType == NODE_TYPE_PLUS || nodes->node->nodeType == NODE_TYPE_MINUS) {
		nodes = nodes->next;
		nodes = factor(nodes);
	}
	else {
		nodes = factor(nodes);
	}
	return nodes;
}

NodeList* factor(NodeList *nodes) {
	if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER
			|| nodes->node->nodeType == NODE_TYPE_NUMBER) {
		printf("found literal %s\n", nodes->node->value);
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
		nodes = nodes->next;
	}
	return nodes;
}


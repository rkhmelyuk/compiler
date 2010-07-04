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

void expression(NodeList *nodes);
NodeList* simpleTerm(NodeList *nodes);
NodeList* term(NodeList *nodes);
NodeList* factor(NodeList *nodes);

void syntaxError(char *error) {
	printf("Syntax error: %s", error);
	exit(SYNTAX_ERROR);
}

void synparse(NodeList *nodes) {
	expression(nodes);
}

void expression(NodeList *nodes) {
	NodeList *list = nodes;
	while ((list = simpleTerm(list)) != NULL);
}

NodeList* simpleTerm(NodeList *nodes) {
	if (nodes->node->nodeType == NODE_TYPE_PLUS || nodes->node->nodeType == NODE_TYPE_MINUS) {
		nodes = nodes->next;
		nodes = term(nodes);
	}
	else if (nodes->node->nodeType == NODE_TYPE_COMMENT) {
		nodes = nodes->next;
	}
	else {
		nodes = term(nodes);
	}
	return nodes;
}

NodeList* term(NodeList *nodes) {
	if (nodes->node->nodeType == NODE_TYPE_MUL || nodes->node->nodeType == NODE_TYPE_DIV) {
		nodes = nodes->next;
		nodes = factor(nodes);
	}
	else if (nodes->node->nodeType == NODE_TYPE_COMMENT) {
		nodes = nodes->next;
	}
	else {
		nodes = factor(nodes);
	}
	return nodes;
}

NodeList* factor(NodeList *nodes) {
	if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER) {
		printf("found identifier %s\n", nodes->node->value);
		nodes = nodes->next;
	}
	else if (nodes->node->nodeType == NODE_TYPE_NUMBER) {
		printf("found number %s\n", nodes->node->value);
		nodes = nodes->next;
	}
	else if (nodes->node->nodeType == NODE_TYPE_LBRACE) {
		nodes = simpleTerm(nodes);
		if (nodes->node->nodeType != NODE_TYPE_RBRACE) {
			syntaxError("error in closing braces");
		}
	}
	else if (nodes->node->nodeType == NODE_TYPE_COMMENT) {
		nodes = nodes->next;
	}
	else {
		syntaxError("Unexpected factor");
	}
	return nodes;
}


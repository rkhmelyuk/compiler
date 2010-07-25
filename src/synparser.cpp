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

AstNode* definition();
AstNode* calculate();
AstNode* simpleTerm();
AstNode* term();
AstNode* factor();
void stabilize();
bool addDefObject(Node *currentNode, DefObject *object);
DefObject* getDefObject(Node *currentNode, char *name);
AstNode* parseNodes();

static bool hasError;
static DefContext *globalContext;
static NodeList *nodes;

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

AstNode* synparse(NodeList *nodeList) {
	hasError = false;
	globalContext = new DefContext;
	globalContext->name = "global";
	nodes = nodeList;

	AstNode *ast = parseNodes();

    if (hasError) {
		printf("COMPILATION FAILED!\n");
		return null;
	}

    return ast;
}

AstNode* parseNodes() {
	AstNode *astNodeList = new AstNode; // points to the begin
	AstNode *astNode = astNodeList;
	astNode->type = SEQUENCE;

    while (nodes != NULL) {
		switch (nodes->node->nodeType) {
			case NODE_TYPE_DEF:
				astNode->left = definition();
				break;
			case NODE_TYPE_CALC:
				nodes = nodes->next;
				astNode->left = calculate();
				break;
			case NODE_TYPE_EOF:
				return astNodeList;
			default:
				unexpected(nodes->node);
				stabilize();
		}
		if (nodes != NULL) {
			nodes = nodes->next;
		}

		// next AST node
		if (astNode->left != null) {
			astNode->right = new AstNode;
			astNode = astNode->right;
			astNode->type = SEQUENCE;
		}
	}

    return astNodeList;
}

bool isStabilizeNodeType(NodeType nodeType) {
	return nodeType == NODE_TYPE_SEMICOLON || nodeType == NODE_TYPE_EOF;
}

void stabilize() {
	do {
		nodes = nodes->next;
		if (nodes != NULL && isStabilizeNodeType(nodes->node->nodeType)) {
			break;
		}
	} while (nodes != NULL);
}

AstNode* definition() {
	AstNode *node = new AstNode;
	node->type = DEF;

	nodes = nodes->next;
	if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER) {
		DefObject *object = new DefObject;
		object->name = nodes->node->value;
		object->type = VARIABLE;

		nodes = nodes->next;
		if (addDefObject(nodes->node, object)) {
			AstNode *left = new AstNode;
			left->value = object->name;
			left->type = IDENT;
			node->left = left;

			if (nodes->node->nodeType == NODE_TYPE_EQUAL) {
				nodes = nodes->next;
				AstNode *right = new AstNode;
				node->right = right;
				if (nodes->node->nodeType == NODE_TYPE_NUMBER) {
					right->value = nodes->node->value;
					right->type = NUMBER;
					nodes = nodes->next;
				}
				else if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER) {
					right->value = getDefObject(nodes->node, nodes->node->value) -> name;
					right->type = IDENT;
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
	return node;
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

AstNode* calculate() {
	AstNode *node = new AstNode;
	node->type = CALC;
	node->left = simpleTerm();

	if (nodes != NULL && nodes->node->nodeType != NODE_TYPE_SEMICOLON) {
		unexpected(nodes->node);
		stabilize();
	}

	return node;
}

AstNode* simpleTerm() {
	AstNode* node = null;

	if (nodes->node->nodeType == NODE_TYPE_PLUS) {
		nodes = nodes->next;
		node = new AstNode;
		node->type = ADD;
	}
	else if (nodes->node->nodeType == NODE_TYPE_MINUS) {
		nodes = nodes->next;
		node = new AstNode;
		node->type = SUB;
	}

	AstNode *termAst= term();
	if (node == null) {
		node = termAst;
	}
	else {
		node->left = termAst;
	}

	while (nodes->node->nodeType == NODE_TYPE_PLUS
			|| nodes->node->nodeType == NODE_TYPE_MINUS) {
		AstNode *cNode = new AstNode;

		if (nodes->node->nodeType == NODE_TYPE_PLUS) {
			cNode->type = ADD;
		}
		else if (nodes->node->nodeType == NODE_TYPE_MINUS) {
			cNode->type = SUB;
		}

		nodes = nodes->next;
		cNode->right = simpleTerm();
		cNode->left = node;

		node = cNode;
	}
	return node;
}

AstNode* term() {
	AstNode *node = factor();

	while (nodes->node->nodeType == NODE_TYPE_MUL
			|| nodes->node->nodeType == NODE_TYPE_DIV) {
		AstNode *cNode= new AstNode;

		if (nodes->node->nodeType == NODE_TYPE_MUL) {
			cNode->type = MUL;
		}
		else if (nodes->node->nodeType == NODE_TYPE_MUL) {
			cNode->type = DIV;
		}

		nodes = nodes->next;
		cNode->right = term();
		cNode->left = node;

		node = cNode;
	}

	return node;
}

AstNode* factor() {
	AstNode *node = null;

	if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER) {
		node = new AstNode;
		node->type = IDENT;
		node->value = getDefObject(nodes->node, nodes->node->value) -> name;
		nodes = nodes->next;
	}
	else if (nodes->node->nodeType == NODE_TYPE_NUMBER) {
		node = new AstNode;
		node->type = NUMBER;
		node->value = nodes->node->value;
		nodes = nodes->next;
	}
	else if (nodes->node->nodeType == NODE_TYPE_LBRACE) {
		nodes = nodes->next;
		do {
			node = simpleTerm();
			if (nodes == NULL || (nodes->node->nodeType != NODE_TYPE_RBRACE && nodes->next == NULL)) {
				syntaxError(nodes->node, "right brace is absent");
				break;
			}
		} while (nodes->node->nodeType != NODE_TYPE_RBRACE);
		nodes = nodes->next;
	}
	else {
		unexpected(nodes->node);
		stabilize();
	}
	return node;
}


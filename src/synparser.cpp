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

AstNode* calculate();
AstNode* definition();
AstNode* assignment();
AstNode* simpleTerm();
AstNode* ifThenElse();
AstNode* comparison();
AstNode* statement();
AstNode* statements();
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
	globalContext->objects = new list<DefObject*>;
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
	astNodeList->left = astNodeList->right = null;
	astNodeList->value = null;

	AstNode *astNode = astNodeList;
	astNode->type = SEQUENCE;

    while (nodes != null && nodes->node != null) {
    	if (nodes->node->nodeType == NODE_TYPE_EOF) {
			break;
    	}

    	astNode->left = statement();

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

AstNode* statement() {
	AstNode* ast = null;

	switch (nodes->node->nodeType) {
		case NODE_TYPE_DEF:
			nodes = nodes->next;
			ast = definition();
			break;
		case NODE_TYPE_CALC:
			nodes = nodes->next;
			ast= calculate();
			break;
		case NODE_TYPE_IDENTIFIER:
			if (nodes->next != null) {
				if (nodes->next->node->nodeType == NODE_TYPE_ASSIGN) {
					// assignment
					ast = assignment();
					break;
				}
			}
			unexpected(nodes->node);
			stabilize();
			break;
		case NODE_TYPE_IF:
			nodes = nodes->next;
			ast = ifThenElse();
			break;
		case NODE_TYPE_EOF:
			break;
		default:
			unexpected(nodes->node);
			stabilize();
	}

	return ast;
}

AstNode* statements() {
	if (nodes->node->nodeType != NODE_TYPE_LBRACE) {
		unexpected(nodes->node);
	}

	AstNode *astNodeList = new AstNode; // points to the begin
	astNodeList->left = astNodeList->right = null;
	astNodeList->value = null;

	AstNode *astNode = astNodeList;
	astNode->type = SEQUENCE;

	nodes = nodes->next;

    while (nodes != null && nodes->node != null) {
    	if (nodes->node->nodeType == NODE_TYPE_EOF) {
			syntaxError(nodes->node, "Unexpected end of file");
    	}
    	else if (nodes->node->nodeType == NODE_TYPE_RBRACE) {
    		break;
    	}

    	astNode->left = statement();

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

    nodes = nodes->next;

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

			if (nodes->node->nodeType == NODE_TYPE_ASSIGN) {
				nodes = nodes->next;
				node->right = simpleTerm();
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

AstNode* assignment() {
	AstNode *left = new AstNode;
	left->value = nodes->node->value;
	left->type = IDENT;

	AstNode *node = new AstNode;
	node->type = ASSIGN;
	node->left = left;

	nodes = nodes->next;
	if (nodes->next != null) {
		nodes = nodes->next;
		node->right = simpleTerm();

		if (nodes->node->nodeType != NODE_TYPE_SEMICOLON) {
			unexpected(nodes->node);
		}
	}
	else {
		unexpected(nodes->node);
		nodes = nodes->next;
	}
	return node;
}

AstNode* ifThenElse() {

	AstNode *node = new AstNode;
	node->type = IF;

	if (nodes->node->nodeType == NODE_TYPE_LPAREN) {
		nodes = nodes->next;

		AstNode *left = new AstNode; // condition
		left = comparison();
		node->left = left;

		if (nodes->node->nodeType == NODE_TYPE_RPAREN) {
			nodes = nodes->next;
			if (nodes->node->nodeType == NODE_TYPE_THEN) {
				AstNode *right = new AstNode;
				right->type = THEN;
				node->right = right;

				nodes = nodes->next;
				if (nodes->node->nodeType == NODE_TYPE_LBRACE) {
					// block
					right->left = statements();
				}
				else {
					right->left = statement();
					nodes = nodes->next;
				}
				if (nodes->node->nodeType == NODE_TYPE_ELSE) {
					nodes = nodes->next;
					if (nodes->node->nodeType == NODE_TYPE_LBRACE) {
						// block
						right->right = statements();
					}
					else {
						right->right = statement();
						nodes = nodes->next;
					}
				}
				//else {
					nodes = nodes->prev;
				//}
			}
			else {
				syntaxError(nodes->node, "'then' is absent");
			}
		}
		else {
			syntaxError(nodes->node, "right parenthesis is absent");
		}
	}
	else {
		syntaxError(nodes->node, "left parenthesis is absent");
	}

	return node;
}

AstNode* comparison() {
	AstNode* node = new AstNode;

	node->left = simpleTerm();

	int nodeType = nodes->node->nodeType;
	nodes = nodes->next;
	node->right = simpleTerm();

	switch (nodeType) {
		case NODE_TYPE_EQUAL:
			node->type = EQUAL;
			break;
		case NODE_TYPE_LESS:
			node->type = LESS;
			break;
		case NODE_TYPE_LESS_EQ:
			node->type = LESS_EQUAL;
			break;
		case NODE_TYPE_GREATER_EQ:
			node->type = GREATER_EQUAL;
			break;
		case NODE_TYPE_GREATER:
			node->type = GREATER_EQUAL;
			break;
		case NODE_TYPE_NOT_EQUAL:
			node->type = NOT_EQUAL;
			break;
		default:
			syntaxError(nodes->node, "wrong condition statement");
	}

	return node;
}

bool addDefObject(Node *currentNode, DefObject *object) {
	for (list<DefObject*>::iterator iter = globalContext->objects->begin(); iter != globalContext->objects->end(); iter++) {
		DefObject *obj = *iter;
		if (strcmp(object->name, obj->name) == 0) {
			// variable is already defined
			syntaxError(currentNode, "variable '%s' is already defined");
			return false;
		}
	}

	globalContext->objects->push_back(object);

	/*if (globalContext->last == null) {
		globalContext->first = node;
		globalContext->last = node;
	}
	else {
		node->prev = globalContext->last;
		globalContext->last = node;
	}*/
	return true;
}

DefObject* getDefObject(Node *currentNode, char *name) {
	for (list<DefObject*>::iterator iter = globalContext->objects->begin(); iter != globalContext->objects->end(); iter++) {
		DefObject *obj = *iter;
		if (strcmp(name, obj->name) == 0) {
			// variable is already defined
			return obj;
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
	else if (nodes->node->nodeType == NODE_TYPE_LPAREN) {
		nodes = nodes->next;
		do {
			node = simpleTerm();
			if (nodes == NULL || (nodes->node->nodeType != NODE_TYPE_RPAREN && nodes->next == NULL)) {
				syntaxError(nodes->node, "right parenthesis is absent");
				break;
			}
		} while (nodes->node->nodeType != NODE_TYPE_RPAREN);
		nodes = nodes->next;
	}
	else {
		unexpected(nodes->node);
		stabilize();
	}
	return node;
}


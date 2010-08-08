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

using namespace std;

AstNode* import();
AstNode* func();
AstNode* funcArgs();
AstNode* funcCall();
AstNode* funcCallArgs();
AstNode* print();
AstNode* definition();
AstNode* assignment();
AstNode* simpleTerm();
AstNode* ifThenElse();
AstNode* whileLoop();
AstNode* breakOp();
AstNode* continueOp();
AstNode* comparison();
AstNode* statement();
AstNode* statements();
AstNode* term();
AstNode* factor();

void stabilize();
AstNode* parseNodes();

static bool hasError;

static NodeList *nodes;
static char *filename;

void syntaxError(Node *node, char *error) {
	printf("Syntax error: %s in %s at position [%d, %d]\n",
			error, filename, node->position->row, node->position->column);
	hasError = true;
}

void unexpected(Node *node) {
	printf("Syntax error: unexpected '%s' in %s at position [%d, %d]\n",
			node->value, filename, node->position->row, node->position->column);
	hasError = true;
}

AstNode* synparse(char* file, NodeList *nodeList) {
	hasError = false;
	filename = file;
	nodes = nodeList;

	AstNode *ast = parseNodes();

    if (hasError) {
		printf("COMPILATION FAILED!\n");
		return null;
	}

    return ast;
}

AstNode* parseNodes() {

	AstNode *astNodeList = new AstNode(filename, nodes->node);
	astNodeList->left = astNodeList->right = null;
	astNodeList->value = null;

	AstNode *astNode = astNodeList;
	astNode->type = SEQUENCE;

	bool importDone = false;

    while (nodes != null && nodes->node != null) {
    	if (nodes->node->nodeType == NODE_TYPE_EOF) {
			break;
    	}

    	astNode->left = statement();

    	if (astNode->left != null && astNode->left->type != IMPORT) {
			importDone = true;
    	}
    	else if (importDone) {
    		syntaxError(nodes->node, "Import is not allowed at this place");
    	}

		if (nodes != NULL) {
			nodes = nodes->next;
		}

		// next AST node
		if (astNode->left != null) {
			astNode->right = new AstNode(filename, nodes->node);
			astNode = astNode->right;
			astNode->type = SEQUENCE;
		}
	}

    return astNodeList;
}

AstNode* statement() {
	AstNode* ast = null;

	switch (nodes->node->nodeType) {
		case NODE_TYPE_AMPERSAT:
			nodes = nodes->next;
			ast = import();
			break;
		case NODE_TYPE_DEF:
			nodes = nodes->next;
			ast = definition();
			break;
		case NODE_TYPE_PRINT:
			nodes = nodes->next;
			ast = print();
			break;
		case NODE_TYPE_FUNC:
			nodes = nodes->next;
			ast = func();
			break;
		case NODE_TYPE_IDENTIFIER:
			if (nodes->next != null) {
				if (nodes->next->node->nodeType == NODE_TYPE_ASSIGN) {
					// assignment
					ast = assignment();
					break;
				}
				if (nodes->next->node->nodeType == NODE_TYPE_LPAREN) {
					// call function
					ast = funcCall();
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
		case NODE_TYPE_WHILE:
			nodes = nodes->next;
			ast = whileLoop();
			break;
		case NODE_TYPE_BREAK:
			nodes = nodes->next;
			ast = breakOp();
			break;
		case NODE_TYPE_CONTINUE:
			nodes = nodes->next;
			ast = continueOp();
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

	AstNode *astNodeList = new AstNode(filename, nodes->node);
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

    	if (astNode->left != null && astNode->left->type == IMPORT) {
			syntaxError(nodes->node, "Import is not allowed at this place");
		}

		if (nodes != NULL) {
			nodes = nodes->next;
		}

		// next AST node
		if (astNode->left != null) {
			astNode->right = new AstNode(filename, nodes->node);
			astNode = astNode->right;
			astNode->type = SEQUENCE;
		}
	}

    nodes = nodes->next;

    return astNodeList;
}

bool isStabilizeNodeType(NodeType nodeType) {
	return nodeType == NODE_TYPE_SEMICOLON
			|| nodeType == NODE_TYPE_LBRACE
			|| nodeType == NODE_TYPE_RBRACE
			|| nodeType == NODE_TYPE_EOF;
}

void stabilize() {
	do {
		nodes = nodes->next;
		if (nodes != NULL && isStabilizeNodeType(nodes->node->nodeType)) {
			break;
		}
	} while (nodes != NULL);
}

AstNode* import() {
	AstNode *node = new AstNode(filename, nodes->node);
	node->type = IMPORT;
	node->value = nodes->node->value;

	nodes = nodes->next;
	if (nodes->node->nodeType != NODE_TYPE_SEMICOLON) {
		syntaxError(nodes->node, "right parenthesis is absent");
		stabilize();
	}

	return node;
}

AstNode* definition() {
	AstNode *node = new AstNode(filename, nodes->node);
	node->type = DEF;

	if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER) {

		AstNode *left = new AstNode(filename, nodes->node);

		DefObject *object = new DefObject;
		object->name = nodes->node->value;
		object->type = VARIABLE;
		object->dataType = TYPE_INTEGER;

		left->value = object;
		left->type = IDENT;

		node->left = left;

		nodes = nodes->next;

		if (nodes->node->nodeType == NODE_TYPE_ASSIGN) {
			nodes = nodes->next;
			node->right = simpleTerm();
		}
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

AstNode* func() {
	AstNode *node = new AstNode(filename, nodes->node);
	node->type = FUNC;

	if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER) {
		DefObject *object = new DefObject;
		object->name = nodes->node->value;
		object->type = FUNCTION;
		object->dataType = TYPE_VOID;

		node->value = object;

		nodes = nodes->next;
		if (nodes->node->nodeType == NODE_TYPE_LPAREN) {
			nodes = nodes->next;

			AstNode *left = new AstNode(filename, nodes->node);
			left = funcArgs();
			node->left = left;

			if (nodes->node->nodeType == NODE_TYPE_RPAREN) {
				nodes = nodes->next;

				if (nodes->node->nodeType == NODE_TYPE_LBRACE) {
					// block
					node->right = statements();
					nodes = nodes->prev;
				}
			}
			else {
				syntaxError(nodes->node, "right parenthesis is absent");
				stabilize();
			}
		}
		else {
			syntaxError(nodes->node, "left parenthesis is absent");
			stabilize();
		}
	}
	else {
		syntaxError(nodes->node, "function name is absent");
		stabilize();
	}


	return node;
}

AstNode* funcArgs() {
	AstNode *begin= new AstNode(filename, nodes->node);
	begin->type = SEQUENCE;

	AstNode *node = begin;
	while (nodes->node->nodeType == NODE_TYPE_DEF) {
		AstNode *defNode= new AstNode(filename, nodes->node);
		defNode->type = DEF;

		nodes = nodes->next;
		if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER) {
			DefObject *object = new DefObject;
			object->name = nodes->node->value;
			object->type = VARIABLE;
			object->dataType = TYPE_INTEGER;

			defNode->left = new AstNode(filename, nodes->node);
			defNode->left->type = IDENT;
			defNode->left->value = object;

			nodes = nodes->next;
			if (nodes->node->nodeType == NODE_TYPE_COMMA) {
				nodes = nodes->next;
			}
		}
		else {
			syntaxError(nodes->node, "argument name is expected");
			continue;
		}

		node->left = defNode;
		node->right = new AstNode(filename, nodes->node);
		node = node->right;
		node->type = SEQUENCE;
	}
	return begin;
}

AstNode* funcCall() {
	AstNode *node = new AstNode(filename, nodes->node);
	node->type = FUNC_CALL;
	node->value = nodes->node->value;

	nodes = nodes->next;
	if (nodes->node->nodeType == NODE_TYPE_LPAREN) {
		nodes = nodes->next;

		AstNode *left = new AstNode(filename, nodes->node);
		left = funcCallArgs();
		node->left = left;

		if (nodes->node->nodeType != NODE_TYPE_RPAREN) {
			syntaxError(nodes->node, "right parenthesis is absent");
			stabilize();
		}
		else {
			nodes = nodes->next;
			if (nodes->node->nodeType != NODE_TYPE_SEMICOLON) {
				syntaxError(nodes->node, "right parenthesis is absent");
				stabilize();
			}
		}
	}
	else {
		syntaxError(nodes->node, "left parenthesis is absent");
		stabilize();
	}

	return node;
}

AstNode* funcCallArgs() {
	AstNode *begin = new AstNode(filename, nodes->node);
	begin->type = SEQUENCE;

	AstNode *node = begin;
	while (nodes != null && nodes->node->nodeType != NODE_TYPE_RPAREN) {

		AstNodeType type;
		if (nodes->node->nodeType == NODE_TYPE_IDENTIFIER) {
			type = IDENT;
		}
		else if (nodes->node->nodeType == NODE_TYPE_STRING) {
			type = STRING;
		}
		else if (nodes->node->nodeType == NODE_TYPE_NUMBER) {
			type = NUMBER;
		}
		else {
			syntaxError(nodes->node, "argument name is expected");
			continue;
		}

		AstNode *argNode= new AstNode(filename, nodes->node);
		argNode->type = type;
		argNode->value = nodes->node->value;

		nodes = nodes->next;
		if (nodes->node->nodeType == NODE_TYPE_COMMA) {
			nodes = nodes->next;
		}

		node->left = argNode;
		node->right = new AstNode(filename, nodes->node);
		node = node->right;
		node->type = SEQUENCE;
	}
	return begin;
}

AstNode* assignment() {
	AstNode *left = new AstNode(filename, nodes->node);
	left->value = nodes->node->value;
	left->type = IDENT;

	AstNode *node = new AstNode(filename, nodes->node);
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

	AstNode *node = new AstNode(filename, nodes->node);
	node->type = IF;

	if (nodes->node->nodeType == NODE_TYPE_LPAREN) {
		nodes = nodes->next;

		AstNode *left = new AstNode(filename, nodes->node); // condition
		left = comparison();
		node->left = left;

		if (nodes->node->nodeType == NODE_TYPE_RPAREN) {
			nodes = nodes->next;
			if (nodes->node->nodeType == NODE_TYPE_THEN) {
				AstNode *right = new AstNode(filename, nodes->node);
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
				if (nodes->node != null && nodes->node->nodeType == NODE_TYPE_ELSE) {
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
				nodes = nodes->prev;
			}
			else {
				syntaxError(nodes->node, "'then' is absent");
				stabilize();
			}
		}
		else {
			syntaxError(nodes->node, "right parenthesis is absent");
			stabilize();
		}
	}
	else {
		syntaxError(nodes->node, "left parenthesis is absent");
		stabilize();
	}

	return node;
}

AstNode* whileLoop() {

	AstNode *node = new AstNode(filename, nodes->node);
	node->type = WHILE;

	if (nodes->node->nodeType == NODE_TYPE_LPAREN) {
		nodes = nodes->next;

		AstNode *left = new AstNode(filename, nodes->node); // condition
		left = comparison();
		node->left = left;

		if (nodes->node->nodeType == NODE_TYPE_RPAREN) {
			nodes = nodes->next;

			if (nodes->node->nodeType == NODE_TYPE_LBRACE) {
				// block
				node->right = statements();
				nodes = nodes->prev;
			}
			else {
				node->right = statement();
			}
		}
		else {
			syntaxError(nodes->node, "right parenthesis is absent");
			stabilize();
		}
	}
	else {
		syntaxError(nodes->node, "left parenthesis is absent");
		stabilize();
	}

	return node;
}

AstNode* breakOp() {
	AstNode* node = new AstNode(filename, nodes->node);
	node->type = BREAK;
	return node;
}

AstNode* continueOp() {
	AstNode* node = new AstNode(filename, nodes->node);
	node->type = CONTINUE;
	return node;
}

AstNode* comparison() {
	AstNode* node = new AstNode(filename, nodes->node);

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


AstNode* print() {
	AstNode *node = new AstNode(filename, nodes->node);
	node->type = PRINT;
	if (nodes->node->nodeType == NODE_TYPE_STRING) {
		node->left = new AstNode(filename, nodes->node);
		node->left->type = STRING;
		node->left->value = nodes->node->value;
		nodes = nodes->next;
	}
	else {
		node->left = simpleTerm();
	}

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
		node = new AstNode(filename, nodes->node);
		node->type = ADD;
	}
	else if (nodes->node->nodeType == NODE_TYPE_MINUS) {
		nodes = nodes->next;
		node = new AstNode(filename, nodes->node);
		node->type = SUB;
	}

	AstNode *termAst = term();
	if (node == null) {
		node = termAst;
	}
	else {
		node->left = termAst;
	}

	while (nodes->node->nodeType == NODE_TYPE_PLUS
			|| nodes->node->nodeType == NODE_TYPE_MINUS) {
		AstNode *cNode = new AstNode(filename, nodes->node);

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
		AstNode *cNode= new AstNode(filename, nodes->node);

		if (nodes->node->nodeType == NODE_TYPE_MUL) {
			cNode->type = MUL;
		}
		else if (nodes->node->nodeType == NODE_TYPE_DIV) {
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
		node = new AstNode(filename, nodes->node);
		node->type = IDENT;
		node->value = nodes->node->value;
		nodes = nodes->next;
	}
	else if (nodes->node->nodeType == NODE_TYPE_NUMBER) {
		node = new AstNode(filename, nodes->node);
		node->type = NUMBER;
		node->value = nodes->node->value;
		nodes = nodes->next;
	}
	else if (nodes->node->nodeType == NODE_TYPE_LPAREN) {
		nodes = nodes->next;
		node = simpleTerm();
		if (nodes != null && nodes->node->nodeType != NODE_TYPE_RPAREN) {
			syntaxError(nodes->node, "right parenthesis is absent");
			stabilize();
		}
		else {
			nodes = nodes->next;
		}
	}
	else {
		unexpected(nodes->node);
		stabilize();
	}
	return node;
}


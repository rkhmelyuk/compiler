/*
 * main.cpp
 *
 *  Created on: Jun 28, 2010
 *      Author: ruslan
 */

#include <stdio.h>

#include "types.h"
#include "lexparser.h"
#include "synparser.h"

void printAstNode(AstNode *ast, int indent);

void printRight(AstNode *ast, int indent) {
	printf("\n");
	for(int i = 0; i < indent; i++) {
		printf(" ");
	}
	printf("right(");
	printAstNode(ast, indent + 3);
	printf(")");
}

void printLeft(AstNode *ast, int indent) {
	printf("\n");
	for(int i = 0; i < indent; i++) {
		printf(" ");
	}
	printf("left(");
	printAstNode(ast, indent + 3);
	printf(")");
}

void printAstNode(AstNode *ast, int indent) {
	if (ast != null) {
		printf("\n");
		for(int i = 0; i < indent; i++) {
			printf(" ");
		}
		char *type;
		switch (ast->type) {
			case SEQUENCE:
				type = "sequence";
				break;
			case DEF:
				type = "def";
				break;
			case CALC:
				type = "calculate";
				break;
			case IDENT:
				type = "var";
				break;
			case NUMBER:
				type = "number";
				break;
			case EQUAL:
				type = "eq";
				break;
			case MUL:
				type = "*";
				break;
			case DIV:
				type = "/";
				break;
			case ADD:
				type = "+";
				break;
			case SUB:
				type = "-";
				break;
			case GROUP:
				type = "()";
				break;
			case 0:
				type = "zero";
				break;
			default:
				type = "unknown";
		}
		printf("[%s:%s]:", type, ast->value);
		printLeft(ast->left, indent);
		printRight(ast->right, indent);
	}
}

int main(int argc, char** argv) {

	printf("Lexical parser started\n");
	NodeList *list = parse(
			"def a = 12;\n"
			"def x = 3;\n"
			"calculate 12 + a + #* this is great *# (23 * 45) + (a * (12 - 23)) + a * x;");
	//

	/*while (list && list->node) {
		printf("(%d)%s\n", list->node->nodeType, list->node->value);
		list = list->next;
	}*/

	printf("Syntatical parser started\n");
	AstNode *ast = synparse(list);

	printAstNode(ast, 0);

	printf("\nFinished!\n");
}

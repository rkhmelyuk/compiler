/*
 * main.cpp
 *
 *  Created on: Jun 28, 2010
 *      Author: ruslan
 */

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <fstream>

#include "ast.h"
#include "types.h"
#include "lexparser.h"
#include "synparser.h"
#include "interpretator.h"
#include "vm.h"
#include "vmcompile.h"

using namespace std;

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
		char *format = "[%s:%s]:";
		switch (ast->type) {
			case SEQUENCE:
				type = "sequence";
				break;
			case ASSIGN:
				type = "assign";
				break;
			case DEF:
				type = "def";
				break;
			case PRINT:
				type = "print";
				break;
			case IDENT:
				type = "var";
				break;
			case NUMBER:
				type = "number";
				format = "[%s:%d]";
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
			case THEN:
				type = "then";
				break;
			case IF:
				type = "if";
				break;
			case ELSE:
				type = "else";
				break;
			case WHILE:
				type = "while";
				break;
			case 0:
				type = "zero";
				break;
			default:
				type = "unknown";
		}
		printf(format, type, ast->value);
		printLeft(ast->left, indent);
		printRight(ast->right, indent);
	}
}

int main(int argc, char** argv) {

	if (argc != 2) {
		printf("No file to compile.");
		exit(1);
	}

	char *fileName = argv[1];
	ifstream file(fileName, ios::in);

	file.seekg(0, ios::end);
	uint32 size = 1 + file.tellg();
	file.seekg(0, ios::beg);

	char* program = new char[size];
	file.read(program, size);

	file.close();

	NodeList *list = parse(program);

	//printf("Syntatical parser started\n");
	AstNode *ast = synparse(fileName, list);

	if (ast != null) {
		ast = semanticCheck(ast);
		if (ast != null) {
			ast = optimize(ast);
			//printAstNode(ast, 0);
			interpretate(ast);

			//byte *instructions = compile(ast);
			//execute(instructions);
		}
	}

	printf("\n\nFinished!\n");
}

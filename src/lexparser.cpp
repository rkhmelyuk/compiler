/*
 * lexparser.cpp
 *
 *  Created on: Jun 28, 2010
 *      Author: Ruslan Khmelyuk
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cctype>
#include "lexparser.h"

#undef getc

char getc(Sym *s);
void revokec(Sym *s);
void lexerError(Sym *s, char *error);
void writePos(Node *node, Sym *s);

NodeList* parse(char *string) {

	NodeList *begin = new NodeList;
	NodeList *nl = begin;

	Sym *s = new Sym;
	s->row = 1;
	s->col = 0;
	s->pos = 0;
	s->string = string;

	char c;
	while ((c = getc(s))) {

		if (isspace(c)) {
			continue;
		}

		if (nl->node) {
			nl->next = new NodeList;
			nl->next->prev = nl;
			nl = nl->next;
			nl->next = NULL;
			nl->node = NULL;
		}

		Node *n;
		switch (c) {
		case '+':
			n = new Node;
			n->value = new char[2];
			n->value[0] = c;
			n->value[1] = 0;
			n->nodeType = NODE_TYPE_PLUS;
			nl->node = n;
			writePos(n, s);
			continue;
		case '-':
			n = new Node;
			n->value = new char[2];
			n->value[0] = c;
			n->value[1] = 0;
			n->nodeType = NODE_TYPE_MINUS;
			nl->node = n;
			writePos(n, s);
			continue;
		case '*':
			n = new Node;
			n->value = new char[2];
			n->value[0] = c;
			n->value[1] = 0;
			n->nodeType = NODE_TYPE_MUL;
			nl->node = n;
			writePos(n, s);
			continue;
		case '/':
			n = new Node;
			n->value = new char[2];
			n->value[0] = c;
			n->value[1] = 0;
			n->nodeType = NODE_TYPE_DIV;
			nl->node = n;
			writePos(n, s);
			continue;
		case '=':
			n = new Node;
			n->value = new char[2];
			n->value[0] = c;
			n->value[1] = 0;
			n->nodeType = NODE_TYPE_EQUAL;
			nl->node = n;
			writePos(n, s);
			continue;
		case ';':
			n = new Node;
			n->value = new char[2];
			n->value[0] = c;
			n->value[1] = 0;
			n->nodeType = NODE_TYPE_SEMICOLON;
			nl->node = n;
			writePos(n, s);
			continue;
		case '(':
			n = new Node;
			n->value = new char[2];
			n->value[0] = c;
			n->value[1] = 0;
			n->nodeType = NODE_TYPE_LBRACE;
			nl->node = n;
			writePos(n, s);
			continue;
		case ')':
			n = new Node;
			n->value = new char[2];
			n->value[0] = c;
			n->value[1] = 0;
			n->nodeType = NODE_TYPE_RBRACE;
			nl->node = n;
			writePos(n, s);
			continue;
		case '#':
			c = getc(s);
			if (c == '*') {
				int commentStartPos = s->pos + 1;
				do {
					c = getc(s);
					if (c == '*') {
						c = getc(s);
						if (c == '#') {
							// comment is closed!
							break;
						}
					}
				} while (c);
			}
			else {
				lexerError(s, "Unexpected symbol.");
			}
			continue;
		}

		if (isdigit(c)) {
			n = new Node();
			n->nodeType = NODE_TYPE_NUMBER;
			n->value = new char[1];
			n->value[0] = 0;
			do {
				char* str = new char[2];
				str[0] = c;
				str[1] = 0;
				strcat(n->value, str);
				c = getc(s);
			} while (isdigit(c));
			if (c) revokec(s);
			nl->node = n;
			writePos(n, s);
		}
		else if (isalpha(c)) {
			n = new Node();
			n->value = new char[1];
			n->value[0] = 0;
			do {
				char* str = new char[2];
				str[0] = c;
				str[1] = 0;
				strcat(n->value, str);
				c = getc(s);
			} while (isalpha(c) || isdigit(c));
			if (c) revokec(s);
			nl->node = n;

			if (strcmp(n->value, "def") == 0) {
				n->nodeType = NODE_TYPE_DEF;
			}
			else if (strcmp(n->value, "calculate") == 0) {
				n->nodeType = NODE_TYPE_CALC;
			}
			else {
				n->nodeType = NODE_TYPE_IDENTIFIER;
			}
			writePos(n, s);
		}
		else {
			lexerError(s, "Unexpected symbol.");
		}
	}

	nl->next = new NodeList;
	nl->next->node = new Node;
	nl->next->prev = nl;
	nl = nl->next;
	nl->node->nodeType = NODE_TYPE_EOF;
	nl->node->value = "eof";
	writePos(nl->node, s);

	return begin;
}

void writePos(Node *node, Sym *s) {
	node->position = new Position;
	node->position->row = s->row;
	node->position->column = s->col;
}

char getc(Sym *s) {
	int len = strlen(s->string);
	int pos = s->pos;
	if (len > pos) {
		char c = s->string[pos];
		if (c == '\n') {
			s->row++;
			s->col = 0;
		}
		s->pos++;
		s->col++;

		return c;
	}
	return NULL;
}

void revokec(Sym *s) {
	s->pos--;
	s->col--;
}

void lexerError(Sym *s, char *error) {
	printf("Error (at %d): %s", s->pos, error);
	exit(LEXER_ERROR);
}

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
#include "types.h"

#undef getc

char getc(Sym *s);
void revokec(Sym *s);
void lexerError(Sym *s, char *error);
void writePos(Node *node, Sym *s);

NodeList* parse(char *string) {

	NodeList *begin = new NodeList;
	begin->next = begin->prev = null;
	begin->node = null;

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
			if (getc(s) == '=') {
				n->value = new char[3];
				n->value[0] = c;
				n->value[1] = c;
				n->value[2] = 0;
				n->nodeType = NODE_TYPE_EQUAL;
			}
			else {
				revokec(s);
				n->value = new char[2];
				n->value[0] = c;
				n->value[1] = 0;
				n->nodeType = NODE_TYPE_ASSIGN;
			}
			nl->node = n;
			writePos(n, s);
			continue;
		case '>':
			n = new Node;
			if (getc(s) == '=') {
				n->value = new char[3];
				n->value[0] = '>';
				n->value[1] = '=';
				n->value[2] = 0;
				n->nodeType = NODE_TYPE_GREATER_EQ;
			}
			else {
				revokec(s);
				n->value = new char[2];
				n->value[0] = '>';
				n->value[1] = 0;
				n->nodeType = NODE_TYPE_GREATER;
			}
			nl->node = n;
			writePos(n, s);
			continue;
		case '!':
			if (getc(s) == '=') {
				n = new Node;
				n->value = new char[3];
				n->value[0] = '!';
				n->value[1] = '=';
				n->value[2] = 0;
				n->nodeType = NODE_TYPE_NOT_EQUAL;
				nl->node = n;
				writePos(n, s);
			}
			else {
				revokec(s);
				lexerError(s, "Unknown construction, did you mean '!='");
			}

			continue;
		case '<':
			n = new Node;
			if (getc(s) == '=') {
				n->value = new char[3];
				n->value[0] = '<';
				n->value[1] = '=';
				n->value[2] = 0;
				n->nodeType = NODE_TYPE_LESS_EQ;
			}
			else {
				revokec(s);
				n->value = new char[2];
				n->value[0] = '<';
				n->value[1] = 0;
				n->nodeType = NODE_TYPE_LESS;
			}
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
				do {
					c = getc(s);
					if (c == '\n') {
						break;
					}
				} while (c);
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
		else if (c == EOF || c == 25) {
			// rollback to previous node and break reading
			nl = nl->prev;
			break;
		}
		else {
			printf("%c", c);
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

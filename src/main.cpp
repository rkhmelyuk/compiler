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

int main(int argc, char** argv) {

	printf("Lexical parser started\n");
	NodeList *list = parse("a + #* this is great *# (23 * 45)");
	//def a = 12;

	/*while (list && list->node) {
		printf("(%d)%s\n", list->node->nodeType, list->node->value);
		list = list->next;
	}*/

	printf("Syntatical parser started\n");
	synparse(list);

	printf("\nFinished!\n");
}

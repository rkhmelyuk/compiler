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
	NodeList *list = parse("def a = 12;\ncalculate a + #* this is great *# (23 * 45) + (a * (12 - 23)) + a + def (;\n def x = 23; ");
	//

	/*while (list && list->node) {
		printf("(%d)%s\n", list->node->nodeType, list->node->value);
		list = list->next;
	}*/

	printf("Syntatical parser started\n");
	synparse(list);

	printf("\nFinished!\n");
}

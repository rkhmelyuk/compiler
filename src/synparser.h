/*
 * synparser.h
 *
 *  Created on: Jul 1, 2010
 *      Author: Ruslan Khmelyuk
 */

#ifndef SYNPARSER_H_
#define SYNPARSER_H_

#include <list>

#include "types.h"
#include "ast.h"
#include "lexparser.h"

#define SYNTAX_ERROR 65

extern AstNode* synparse(char *filename, NodeList *nodes);

#endif /* SYNPARSER_H_ */

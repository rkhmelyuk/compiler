/*
 * synparser.h
 *
 *  Created on: Jul 1, 2010
 *      Author: Ruslan Khmelyuk
 */

#ifndef SYNPARSER_H_
#define SYNPARSER_H_

#include "types.h"
#include "lexparser.h"

#define SYNTAX_ERROR 65

extern void synparse(NodeList *nodes);

#endif /* SYNPARSER_H_ */

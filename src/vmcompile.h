/*
 * vmcompile.h
 *
 *  Created on: Jul 31, 2010
 *      Author: ruslan
 */

#ifndef VMCOMPILE_H_
#define VMCOMPILE_H_

#include "types.h"
#include "ast.h"
#include "vm.h"

extern byte* compile(AstNode* ast);

#endif /* VMCOMPILE_H_ */

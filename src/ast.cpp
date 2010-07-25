/*
 * ast.cpp
 *
 *  Created on: Jul 25, 2010
 *      Author: ruslan
 */

#include <stdlib.h>

#include "ast.h"
#include "types.h"

AstNode* stringToNumbers(AstNode* ast);
AstNode* foldConstants(AstNode* ast);

AstNode* optimize(AstNode* ast) {
	ast = stringToNumbers(ast);
	ast = foldConstants(ast);

	return ast;
}

AstNode* stringToNumbers(AstNode* ast) {
	if (ast == null) return null;

	if (ast->type == NUMBER) {
		int value = atoi((char*)ast->value);
		ast->value = (void*) value;
	}
	else {
		ast->left = stringToNumbers(ast->left);
		ast->right = stringToNumbers(ast->right);
	}
	return ast;
}

AstNode* foldConstants(AstNode* ast) {
	if (ast == null) return null;

	ast->left = foldConstants(ast->left);
	ast->right= foldConstants(ast->right);

	if (ast->left != null && ast->right != null && ast->left->type == NUMBER && ast->right->type == NUMBER) {
		int32 left = (int32) ast->left->value;
		int32 right= (int32) ast->right->value;
		switch (ast->type) {
			case ADD:
				ast->value = (void*) left + right;
				break;
			case SUB:
				ast->value = (void*) left - right;
				break;
			case MUL:
				ast->value = (void*) (left * right);
				break;
			case DIV:
				ast->value = (void*) (left / right);
				break;
			default:
				return ast;
		}

		ast->type = NUMBER;

		delete ast->left;
		delete ast->right;
		ast->left = null;
		ast->right= null;
	}

	return ast;
}

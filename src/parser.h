#ifndef PARSER_H
#define PARSER_H

#include "node.h"
#include "token.h"

Node* parser_build_syntax_tree(Token* token);
void parser_show_syntax_tree(Node* root);

#endif

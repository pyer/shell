#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "syntax.h"

Node* parser_build_syntax_tree(token_t* token);
void parser_show_syntax_tree(Node* root);

#endif

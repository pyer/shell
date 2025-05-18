#ifndef LEXER_H
#define LEXER_H

#include "token.h"

enum {
  STATE_GENERAL,
  STATE_IN_DQUOTE,
  STATE_IN_QUOTE,
  STATE_IN_EQUAL,
};

Token* lexer(char* input, int size);
void delete_token(Token* ptr);
#endif

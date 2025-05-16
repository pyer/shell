#ifndef LEXER_H
#define LEXER_H

#include "token.h"

// String delimiter
enum {
  STATE_GENERAL,
  STATE_IN_DQUOTE,
  STATE_IN_QUOTE,
  STATE_IN_EQUAL,
};

token_t* lexer_build(char* input, int size);
void lexer_destroy(token_t* ptr);
void lexer_show(token_t* ptr);
#endif

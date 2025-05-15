#ifndef LEXER_H
#define LEXER_H

// Token type
enum {
  TT_DEFAULT  = '$',
  TT_PIPE     = '|',
  TT_LESSER   = '<',
  TT_GREATER  = '>',
  TT_DOUBLE_GREATER = 0x3E3E, // '>',
  TT_EQUAL    = '=',
  TT_PLUS     = '+',
  TT_MINUS    = '-',
  TT_MULTIPLY = '*',
  TT_DIVIDE   = '/',
  TT_PERCENT  = '%'
};

// String delimiter
enum {
  STATE_GENERAL,
  STATE_IN_DQUOTE,
  STATE_IN_QUOTE,
};

typedef struct token token_t;
struct token {
  int type;
  char* data;
  token_t* next;
};

token_t* lexer_build(char* input, int size);
void lexer_destroy(token_t* ptr);
void lexer_show(token_t* ptr);
#endif

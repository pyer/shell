#ifndef TOKEN_H
#define TOKEN_H

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

typedef struct token Token;
struct token {
  int type;
  char* data;
  Token* next;
};

#endif

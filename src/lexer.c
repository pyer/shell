#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

Token* create_first_token(int size)
{
  Token* tok = malloc(sizeof(Token));
  tok->type = TT_DEFAULT;
  tok->data = malloc(size + 1); // 1 for null terminator
  tok->data[0] = 0;
  tok->next = NULL;
  return tok;
}

Token* create_next_token(Token* tok, int size)
{
  tok->next = malloc(sizeof(Token));
  tok = tok->next;
  tok->type = TT_DEFAULT;
  tok->data = malloc(size + 1); // 1 for null terminator
  tok->data[0] = 0;
  tok->next = NULL;
  return tok;
}

Token* create_token(char* data)
{
  Token* tok = malloc(sizeof(Token));
  tok->type = TT_DEFAULT;
  tok->data = malloc(strlen(data) + 1); // 1 for null terminator
  strcpy(tok->data, data);
  tok->next = NULL;
  return tok;
}

void delete_token(Token* tok) {
  if (tok != NULL) {
    free(tok->data);
    delete_token(tok->next);
    free(tok);
  }
}

Token* lexer(char* input, int size)
{
  if (size == 0) {
    return NULL;
  }

  // allocate the first token
  Token* first_token = create_first_token(size);
  Token* token = first_token;

  int i = 0;
  int j = 0;
  char c;
  int state = STATE_GENERAL;

  do {
    c = input[i];

    if (state == STATE_GENERAL) {
      switch (c) {
        case '\'':
          state = STATE_IN_QUOTE;
          token->data[j++] = c;
          token->type = TT_DEFAULT;
          break;
        case '\"':
          state = STATE_IN_DQUOTE;
          token->data[j++] = c;
          token->type = TT_DEFAULT;
          break;
        case '\\':
          token->data[j++] = input[++i];
          token->type = TT_DEFAULT;
          break;
        case '\n':
        case '\r':
        case '\t':
          break;
        case ' ':
          // end the token that was being read before
          if (j > 0) {
            token->data[j] = 0;
            token = create_next_token(token, size - i);
            token->type = TT_DEFAULT;
            j = 0;
          }
          break;

        case '=':
          state = STATE_IN_EQUAL;
          token->type = TT_EQUAL;
          token->data[j] = 0;
          token = create_next_token(token, size - i);
          j = 0;
          break;

        case '>':
          if (j > 0) {
            token->data[j] = 0;
            token = create_next_token(token, size - i);
          }
          token->type = TT_GREATER;
          // check if ">>"
          if (input[i+1] == c) {
            i++;
            token->type = TT_DOUBLE_GREATER;
          }
          j = 0;
          break;

        case '<':
          if (j > 0) {
            token->data[j] = 0;
            token = create_next_token(token, size - i);
          }
          token->type = TT_LESSER;
          j = 0;
          break;

        case '|':
          if (j > 0) {
            token->data[j] = 0;
            token = create_next_token(token, size - i);
            j = 0;
          }
          // next token
          token->data[0] = c;
          token->data[1] = 0;
          token->type = TT_PIPE;
          token = create_next_token(token, size - i);
          break;

        default:
          token->data[j++] = c;
          break;
      }
    } else if (state == STATE_IN_EQUAL) {
      if (c<' ') {
        state = STATE_GENERAL;
      } else {
        token->data[j++] = c;
      }
    } else if (state == STATE_IN_DQUOTE) {
      token->data[j++] = c;
      if (c == '\"')
        state = STATE_GENERAL;

    } else if (state == STATE_IN_QUOTE) {
      token->data[j++] = c;
      if (c == '\'')
        state = STATE_GENERAL;
    }

    i++;
  } while (c != '\0');

  // terminate data of the last token 
  token->data[j] = 0;
  // and return the first token
  return first_token;
}


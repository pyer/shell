#include <glob.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

void strip_quotes(char* src, char* dest)
{
  int n = strlen(src);
  if (n <= 1) {
    strcpy(dest, src);
    return;
  }

  int i;

  char lastquote = 0;
  int j = 0;

  for (i=0; i < n; i++)
  {
    char c = src[i];
    if ((c == '\'' || c == '\"') && lastquote == 0)
      lastquote = c;
    else if (c == lastquote)
      lastquote = 0;
    else
      dest[j++] = c;
  }

  dest[j] = 0;
}

token_t* create_first_token(int size)
{
  token_t* tok = malloc(sizeof(token_t));
  tok->type = TT_DEFAULT;
  tok->data = malloc(size + 1); // 1 for null terminator
  tok->data[0] = 0;
  tok->next = NULL;
  return tok;
}

token_t* create_next_token(token_t* tok, int size)
{
  tok->next = malloc(sizeof(token_t));
  tok = tok->next;
  tok->type = TT_DEFAULT;
  tok->data = malloc(size + 1); // 1 for null terminator
  tok->data[0] = 0;
  tok->next = NULL;
  return tok;
}

token_t* create_token(char* data)
{
  token_t* tok = malloc(sizeof(token_t));
  tok->type = TT_DEFAULT;
  tok->data = malloc(strlen(data) + 1); // 1 for null terminator
  strcpy(tok->data, data);
  tok->next = NULL;
  return tok;
}

void delete_token(token_t* tok) {
  if (tok != NULL) {
    free(tok->data);
    delete_token(tok->next);
    free(tok);
  }
}

token_t* lexer_build(char* input, int size)
{
  if (size == 0) {
    return NULL;
  }

  // allocate the first token
  token_t* first_token = create_first_token(size);
  token_t* token = first_token;

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
            j = 0;
          }
          break;

        case '>':
          if (j > 0) {
            token->data[j] = 0;
            token = create_next_token(token, size - i);
            j = 0;
          }
          // next token
          token->data[0] = c;
          token->data[1] = 0;
          token->type = TT_GREATER;
          // check if ">>"
          if (input[i+1] == c) {
            token->data[1] = c;
            token->data[2] = 0;
            i++;
            token->type = TT_DOUBLE_GREATER;
          }
          token = create_next_token(token, size - i);
          break;

        case '<':
          if (j > 0) {
            token->data[j] = 0;
            token = create_next_token(token, size - i);
            j = 0;
          }
          // next token
          token->data[0] = c;
          token->data[1] = 0;
          token->type = TT_LESSER;
          token = create_next_token(token, size - i);
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
          token->type = TT_DEFAULT;
          break;
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

    if (c == '\0') {
//      if (j > 0) {
        token->data[j] = 0;
        j = 0;
//      }
    }

    i++;
  } while (c != '\0');

  while (token != NULL) {
    if (token->type == TT_DEFAULT) {
      glob_t globbuf;
      glob(token->data, GLOB_TILDE, NULL, &globbuf);

      if (globbuf.gl_pathc > 0)
      {
        // save the next token so we can attach it later
        token_t* saved = token->next;

        // replace the current token with the first one
        free(token->data);
        token->data = malloc(strlen(globbuf.gl_pathv[0]) + 1);
        strcpy(token->data, globbuf.gl_pathv[0]);

        int i;
        for (i = 1; i < globbuf.gl_pathc; i++) {
          token = create_token(globbuf.gl_pathv[i]);
        }
        token->next = saved;
      } else {
        // token from the user might be inside quotation to escape special characters
        // hence strip the quotation symbol
        char* stripped = malloc(strlen(token->data) + 1);
        strip_quotes(token->data, stripped);
        free(token->data);
        token->data = stripped;
      }
    }

    token = token->next;
  }

  return first_token;
}

void lexer_destroy(token_t* ptr)
{
  if (ptr != NULL)
    delete_token(ptr);
}

/* For debugging */
/*
void lexer_show(token_t* ptr)
{
  while (ptr != NULL) {
    printf("%c : %zu --> %zu '%s'\n", ptr->type, ptr, ptr->next, ptr->data);
    ptr = ptr->next;
  }
}
*/


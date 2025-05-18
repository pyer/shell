#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "lexer.h"
#include "parser.h"
#include "execute.h"
#include "signals.h"

//#define DEBUG

char *prompt = "> ";

void interpret_line(char* linebuffer, size_t len) {
  Token* first_token = NULL;
  Node* root = NULL;

  // lexically analyze and build a list of tokens
  first_token = lexer(linebuffer, len);
  #ifdef DEBUG
    Token* ptr = first_token;
    while (ptr != NULL) {
      printf("Token %c : %zu --> %zu '%s'\n", ptr->type, ptr, ptr->next, ptr->data);
      ptr = ptr->next;
    }
  #endif
  // parse the tokens into an abstract syntax tree
  root = parser_build_syntax_tree(first_token);
  #ifdef DEBUG
    parser_show_syntax_tree(root);
  #endif
  if (root != NULL) {
  #ifdef DEBUG
      printf("EXECUTE : left=%zu right=%zu data='%s'\n", root->left, root->right, root->szData);
  #endif
      // interpret the syntax tree
      execute_syntax_tree(root);
      // free the structures
      deleteNode(root);
  }
  delete_token(first_token);
}

bool getkeyboard(char **buffer, size_t *n)
{
  bool again = true;
  ssize_t nread = 0;
  // keep getline in a loop in case interruption occurs
  while (again) {
    again = false;
    // Print the prompt
    fputs(prompt, stdout);
    nread = getline(buffer, n, stdin);
    if (nread <= 0 && errno == EINTR) {
        again = true;     // signal interruption, read again
        clearerr(stdin);  // clear the error
    }
  }
  // user pressed ctrl-D aka EOF
  if (feof(stdin)) {
    puts("EOF");
    return false;
  }
  return true;
}

/* Entry point */
int main()
{
  char* linebuffer = NULL;
  size_t  len = 0;
  // ignore Ctrl-\ Ctrl-C Ctrl-Z signals
  ignore_signals();

  if (isatty(STDIN_FILENO)) {
    // Read the keyboard
    while (getkeyboard(&linebuffer, &len)) {
      // user pressed ctrl-D aka EOF
      if (feof(stdin)) {
        puts("EOF");
      } else {
        interpret_line(linebuffer, len);
      }
    }
  } else {
    // Read the input file
    while (getline(&linebuffer, &len, stdin) != -1) {
        interpret_line(linebuffer, len);
    }
  }
  free(linebuffer);
  return(last_status());
}


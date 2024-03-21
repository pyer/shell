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

char *prompt = "> ";

void interpret_line(char* linebuffer, size_t len) {
  token_t *first_token = NULL;
  TreeNode* root = NULL;

  // lexically analyze and build a list of tokens
  first_token = lexer_build(linebuffer, len);
// lexer_show(first_token);
  // parse the tokens into an abstract syntax tree
  root = parser_build_syntax_tree(first_token);
//  parser_show_syntax_tree(root);
  if (root != NULL) {
      // interpret the syntax tree
      execute_syntax_tree(root);
      // free the structures
      deleteNode(root);
  }
  lexer_destroy(first_token);
}

bool getkeyboard(char **buffer, size_t *n, FILE *tty)
{
  bool again = true;
  ssize_t nread = 0;
  // keep getline in a loop in case interruption occurs
  while (again) {
    again = false;
    // Print the prompt
    printf(prompt);
    nread = getline(buffer, n, tty);
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
    while (getkeyboard(&linebuffer, &len, stdin)) {
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
}


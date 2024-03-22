#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

/*** Shell grammar ***
 *
  <command line>    ::= <command> '|' <command line>
                      | <command>

  <command>         ::= <simple command> '<' <filename>
                      | <simple command> '>' <filename>
                      | <simple command> ">>" <filename>
                      | <simple command>

  <simple command>  ::= <pathname> <token list>

  <token list>      ::= <token> <token list>
                      | (EMPTY)

  <token> ::= (ANY CHARACTER EXCEPT SPACE, NEWLINE, '|', '<', '>')
 *
 *
**/

Node* CMD();      // test all command production orderwise
Node* CMD1();     // <simple command> '<' <filename>
Node* CMD2();     // <simple command> '>' <filename>
Node* CMD3();     // <simple command> ">>" <filename>

Node* SIMPLECMD();
Node* TOKENLIST();
Node* TOKENLIST1();  //  <token> <token list>
Node* TOKENLIST2();  //  EMPTY

// current_token token pointer
token_t* current_token = NULL;

bool term(int token_type, char** bufferptr)
{
  if (current_token == NULL)
    return false;

  //printf("term: %d - %d\n", current_token->type, token_type);
  if (current_token->type == token_type) {
    if (bufferptr != NULL) {
      *bufferptr = malloc(strlen(current_token->data) + 1);
      strcpy(*bufferptr, current_token->data);
    }
    current_token = current_token->next;
    return true;
  }

  current_token = current_token->next;
  return false;
}

Node* CMD()
{
    token_t* save = current_token;
    Node* node = CMD1();
    if (node != NULL)
        return node;

    current_token = save;
    node = CMD2();
    if (node != NULL)
        return node;

    current_token = save;
    node = CMD3();
    if (node != NULL)
        return node;

    current_token = save;
    return SIMPLECMD();
}

Node* CMD1()
{
  Node* simplecmdNode = SIMPLECMD();
  if (simplecmdNode == NULL)
    return NULL;

  if (!term(TT_LESSER, NULL)) {
    deleteNode(simplecmdNode);
    return NULL;
  }

  char* filename;
  if (!term(TT_TOKEN, &filename)) {
    free(filename);
    deleteNode(simplecmdNode);
    return NULL;
  }

  return createNodeRedirectIn(filename, simplecmdNode);
}

Node* CMD2()
{
  Node* simplecmdNode;
  if ((simplecmdNode = SIMPLECMD()) == NULL)
    return NULL;

  if (!term(TT_GREATER, NULL)) {
    deleteNode(simplecmdNode);
    return NULL;
  }

  char* filename;
  if (!term(TT_TOKEN, &filename)) {
    free(filename);
    deleteNode(simplecmdNode);
    return NULL;
  }

  return createNodeRedirectOut(filename, simplecmdNode);
}

Node* CMD3()
{
  Node* simplecmdNode;
  if ((simplecmdNode = SIMPLECMD()) == NULL)
    return NULL;

  if (!term(TT_DOUBLE_GREATER, NULL)) {
    deleteNode(simplecmdNode);
    return NULL;
  }

  char* filename;
  if (!term(TT_TOKEN, &filename)) {
    free(filename);
    deleteNode(simplecmdNode);
    return NULL;
  }

  return createNodeRedirectOutAppend(filename, simplecmdNode);
}

Node* SIMPLECMD()
{
    char* pathname;
    if (!term(TT_TOKEN, &pathname))
        return NULL;

    Node* tokenListNode = TOKENLIST();
    // we don't check whether tokenlistNode is NULL since its a valid grammer
    return createNodeCommand(pathname, tokenListNode);
}

Node* TOKENLIST()
{
    token_t* save = current_token;

    Node* node;

    if ((current_token = save, node = TOKENLIST1()) != NULL)
        return node;

    if ((current_token = save, node = TOKENLIST2()) != NULL)
        return node;

    return NULL;
}

Node* TOKENLIST1()
{
    Node* tokenListNode;
    Node* result;

    char* arg;
    if (!term(TT_TOKEN, &arg))
        return NULL;

    tokenListNode = TOKENLIST();
    // we don't check whether tokenlistNode is NULL since its a valid grammer
    result = createNodeArgument(arg, tokenListNode);
    return result;
}

Node* TOKENLIST2()
{
    return NULL;
}

Node* parse_tokens()
{
    Node* cmdNode;
    Node* jobNode;
    token_t* save = current_token;

    cmdNode = CMD();
    if (cmdNode == NULL) {
      current_token = save;
      cmdNode = CMD();
      return cmdNode;
    }

    if (!term(TT_PIPE, NULL)) {
      deleteNode(cmdNode);
      current_token = save;
      cmdNode = CMD();
      return cmdNode;
    }

    // recursive parsing
    jobNode = parse_tokens();
    if (jobNode == NULL) {
      deleteNode(cmdNode);
      current_token = save;
      cmdNode = CMD();
      return cmdNode;
    }
    return createNodePipe(cmdNode, jobNode);
}

Node* parser_build_syntax_tree(token_t* token)
{
    if (token == NULL)
      return NULL;
    if (strlen(token->data) == 0)
      return NULL;

    current_token = token;

    Node* syntax_tree = parse_tokens();

    if (current_token != NULL && current_token->type != TT_TOKEN) {
        printf("Syntax Error near: %s\n", current_token->data);
        return NULL;
    }

  return syntax_tree;
}

void parser_show_syntax_tree(Node* ptr)
{
  if (ptr != NULL) {
    printf("%d : %zu --> left=%zu right=%zu data='%s'\n", ptr->type, ptr, ptr->left, ptr->right, ptr->szData);
    parser_show_syntax_tree(ptr->left);
    parser_show_syntax_tree(ptr->right);
  }

}


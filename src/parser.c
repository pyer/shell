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

// current_token token pointer
token_t* current_token = NULL;

bool type_is(int token_type)
{
  if (current_token == NULL)
    return false;

  bool ret = (current_token->type == token_type);
  current_token = current_token->next;
  return ret;
}

bool default_token()
{
  if (current_token == NULL)
    return false;
  if (current_token->type == TT_DEFAULT) {
    return true;
  }
  current_token = current_token->next;
  return false;
}

/*
 * Create the list of arguments from the list of tokens
 */
Node* TOKENLIST()
{
    token_t* save = current_token;
    Node* node = NULL;

    if (default_token()) {
        current_token = current_token->next;
        // we don't check whether TOKENLIST is NULL since its a valid grammer
        node = createNodeArgument(save->data, TOKENLIST());
    }
    if (node == NULL)
        current_token = save;
    return node;
}

Node* SIMPLECMD()
{
    token_t* save = current_token;
    if (default_token()) {
        current_token = current_token->next;
        // we don't check whether TOKENLIST is NULL since its a valid grammer
        return createNodeCommand(save->data, TOKENLIST());
    }
    return NULL;
}

Node* CMD()
{
    token_t* save = current_token;
    Node* node = SIMPLECMD();
    if (node != NULL) {
      if (type_is(TT_LESSER)) {
        if (default_token()) {
          char* filename = current_token->data;
          current_token  = current_token->next;
          return createNodeRedirectIn(filename, node);
        }
      }
      deleteNode(node);
    }

    current_token = save;
    node = SIMPLECMD();
    if (node != NULL) {
      if (type_is(TT_GREATER)) {
        if (default_token()) {
          char* filename = current_token->data;
          current_token  = current_token->next;
          return createNodeRedirectOut(filename, node);
        }
      }
      deleteNode(node);
    }

    current_token = save;
    node = SIMPLECMD();
    if (node != NULL) {
      if (type_is(TT_DOUBLE_GREATER)) {
        if (default_token()) {
          char* filename = current_token->data;
          current_token  = current_token->next;
          return createNodeRedirectOutAppend(filename, node);
        }
      }
      deleteNode(node);
    }

    current_token = save;
    return SIMPLECMD();
}

Node* parse_tokens()
{
    token_t* save = current_token;

    Node* cmdNode = CMD();
    if (cmdNode != NULL) {
      if (type_is(TT_PIPE)) {
        // recursive parsing
        Node* jobNode = parse_tokens();
        if (jobNode != NULL) {
          return createNodePipe(cmdNode, jobNode);
        }
      }
      deleteNode(cmdNode);
    }

    current_token = save;
    cmdNode = CMD();
    return cmdNode;
}

Node* parser_build_syntax_tree(token_t* token)
{
    if (token == NULL)
      return NULL;
    if (strlen(token->data) == 0)
      return NULL;

    current_token = token;

    Node* syntax_tree = parse_tokens();

    if (current_token != NULL && current_token->type != TT_DEFAULT) {
        printf("Syntax Error near: %s\n", current_token->data);
        return NULL;
    }

  return syntax_tree;
}
/*
void parser_show_syntax_tree(Node* ptr)
{
  if (ptr != NULL) {
    printf("%d : %zu --> left=%zu right=%zu data='%s'\n", ptr->type, ptr, ptr->left, ptr->right, ptr->szData);
    parser_show_syntax_tree(ptr->left);
    parser_show_syntax_tree(ptr->right);
  }

}
*/

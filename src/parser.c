
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "node.h"
#include "parser.h"

/*** Shell grammar ***
 *
  <text>            ::= <command>
                      | <command>  '|' <text>
                      | <variable> '=' <value>

  <command>         ::= <simple command> '<' <filename>
                      | <simple command> '>' <filename>
                      | <simple command> ">>" <filename>
                      | <simple command>

  <variable>        ::= <token list>

  <simple command>  ::= <pathname> <token list>

  <token list>      ::= <token> <token list>
                      | (EMPTY)

  <token> ::= (ANY CHARACTER EXCEPT SPACE, NEWLINE, '|', '=', '<', '>')
 *
 *
**/

Node* createNodePipe(Node* leftNode, Node* rightNode)
{
  Node* node = malloc(sizeof(*node));
  assert(node != NULL);
  node->type  = NODE_PIPE;
  node->left  = leftNode;
  node->right = rightNode;
  node->szData = NULL;
  return node;
}

Node* createNodeRedirectIn(char* data, Node* rightNode)
{
  Node* node = malloc(sizeof(*node));
  assert(node != NULL);
  node->type  = NODE_REDIRECT_IN;
  node->left  = NULL;
  node->right = rightNode;
  node->szData = data;
  return node;
}

Node* createNodeRedirectOut(char* data, Node* rightNode)
{
  Node* node = malloc(sizeof(*node));
  assert(node != NULL);
  node->type  = NODE_REDIRECT_OUT;
  node->left  = NULL;
  node->right = rightNode;
  node->szData = data;
  return node;
}

Node* createNodeRedirectOutAppend(char* data, Node* rightNode)
{
  Node* node = malloc(sizeof(*node));
  assert(node != NULL);
  node->type  = NODE_REDIRECT_OUT_APPEND;
  node->left  = NULL;
  node->right = rightNode;
  node->szData = data;
  return node;
}

Node* createNodeArgument(char *argument, Node* tokenListNode)
{
  Node* node = malloc(sizeof(*node));
  assert(node != NULL);
  node->type  = NODE_ARGUMENT;
  node->left  = NULL;
  node->right = tokenListNode;
  node->szData = argument;
  return node;
}

Node* createNodeCommand(char *command, Node* tokenListNode)
{
  Node* node = malloc(sizeof(*node));
  assert(node != NULL);
  node->type  = NODE_COMMAND;
  node->left  = NULL;
  node->right = tokenListNode;
  node->szData = command;
  return node;
}

Node* createNodeVariable(char *variable, Node* value)
{
  Node* node = malloc(sizeof(*node));
  assert(node != NULL);
  node->type  = NODE_VARIABLE;
  node->left  = NULL;
  node->right = value;
  node->szData = variable;
  return node;
}

void deleteNode(Node* node)
{
  if (node != NULL) {
//    if (node->szData != NULL) {
//      free(node->szData);
//    }
    deleteNode(node->left);
    deleteNode(node->right);
    free(node);
  }
}


Token* current_token = NULL;

/*
 * Create the list of arguments from the list of tokens
 */
Node* tokens_list()
{
    Node* node = NULL;
    current_token = current_token->next;
    while (current_token != NULL && current_token->type == TT_DEFAULT) {
        node = createNodeArgument(current_token->data, node);
        current_token = current_token->next;
    }
    return node;
}

/*
 * Parse all tokens to build nodes tree
 */
Node* parse_tokens()
{
    Node* node = NULL;
    while (current_token != NULL) {
      //printf("parse_tokens %c : %zu --> %zu '%s'\n", current_token->type, current_token, current_token->next, current_token->data);
      switch (current_token->type) {
        case TT_PIPE:
          current_token = current_token->next;
          // recursive parsing
          Node* jobNode = parse_tokens();
          if (jobNode != NULL) {
            return createNodePipe(node, jobNode);
          }
          break;
        case TT_GREATER:
          node = createNodeRedirectOut(current_token->data, node);
          current_token = current_token->next;
          break;
        case TT_DOUBLE_GREATER:
          node = createNodeRedirectOutAppend(current_token->data, node);
          current_token = current_token->next;
          break;
        case TT_LESSER:
          node = createNodeRedirectIn(current_token->data, node);
          current_token = current_token->next;
          break;
        default:
          // we don't check whether tokens_list is NULL since it's a valid grammer
//    printf("SIMPLECMD enter %c : %zu --> %zu '%s'\n", current_token->type, current_token, current_token->next, current_token->data);
          Token *cmd = current_token;
          node = createNodeCommand(cmd->data, tokens_list());
          //node = createNodeCommand(current_token->data, tokens_list());
//    printf("SIMPLECMD exit %zu\n", node);
          break;
      }
    }
    return node;
}

Node* parser_build_syntax_tree(Token* token)
{
    if (token == NULL)
      return NULL;
    if (strlen(token->data) == 0)
      return NULL;

    current_token = token;
    if (token->type == TT_EQUAL) {
        current_token = token->next;
        return createNodeVariable(token->data, createNodeArgument(current_token->data, NULL));
    }

    Node* syntax_tree = parse_tokens();

    if (current_token != NULL && current_token->type != TT_DEFAULT) {
        printf("Syntax error near: %s\n", current_token->data);
        return NULL;
    }
    return syntax_tree;
}

void parser_show_syntax_tree(Node* ptr)
{
  if (ptr != NULL) {
    printf("NODE %d : %zu --> left=%zu right=%zu data='%s'\n", ptr->type, ptr, ptr->left, ptr->right, ptr->szData);
    parser_show_syntax_tree(ptr->left);
    parser_show_syntax_tree(ptr->right);
  }
}


#include <assert.h>
#include <stdlib.h>

#include "syntax.h"

/* Syntax tree management */


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

void deleteNode(Node* node)
{
  if (node != NULL) {
    if (node->szData != NULL) {
      free(node->szData);
    }
    deleteNode(node->left);
    deleteNode(node->right);
    free(node);
  }
}


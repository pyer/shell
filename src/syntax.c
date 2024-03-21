#include <assert.h>
#include <stdlib.h>

#include "syntax.h"

/* Syntax tree management */


TreeNode* createNodePipe(TreeNode* leftNode, TreeNode* rightNode)
{
  TreeNode* node = malloc(sizeof(*node));
  assert(node != NULL);
  node->type  = NODE_PIPE;
  node->left  = leftNode;
  node->right = rightNode;
  node->szData = NULL;
  return node;
}

TreeNode* createNodeRedirectIn(char* data, TreeNode* rightNode)
{
  TreeNode* node = malloc(sizeof(*node));
  assert(node != NULL);
  node->type  = NODE_REDIRECT_IN;
  node->left  = NULL;
  node->right = rightNode;
  node->szData = data;
  return node;
}

TreeNode* createNodeRedirectOut(char* data, TreeNode* rightNode)
{
  TreeNode* node = malloc(sizeof(*node));
  assert(node != NULL);
  node->type  = NODE_REDIRECT_OUT;
  node->left  = NULL;
  node->right = rightNode;
  node->szData = data;
  return node;
}

TreeNode* createNodeArgument(char *argument, TreeNode* tokenListNode)
{
  TreeNode* node = malloc(sizeof(*node));
  assert(node != NULL);
  node->type  = NODE_ARGUMENT;
  node->left  = NULL;
  node->right = tokenListNode;
  node->szData = argument;
  return node;
}

TreeNode* createNodeCommand(char *command, TreeNode* tokenListNode)
{
  TreeNode* node = malloc(sizeof(*node));
  assert(node != NULL);
  node->type  = NODE_COMMAND;
  node->left  = NULL;
  node->right = tokenListNode;
  node->szData = command;
  return node;
}

void deleteNode(TreeNode* node)
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


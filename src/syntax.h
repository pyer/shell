#ifndef SYNTAX_H
#define SYNTAX_H

enum {
    NODE_PIPE,
    NODE_REDIRECT_IN,
    NODE_REDIRECT_OUT,
    NODE_COMMAND,
    NODE_ARGUMENT,
};

typedef struct TreeNode
{
    int type;
    char* szData;
    struct TreeNode* left;
    struct TreeNode* right;

} TreeNode;

TreeNode* createNodePipe(TreeNode* leftNode, TreeNode* rightNode);
TreeNode* createNodeRedirectIn(char* data, TreeNode* rightNode);
TreeNode* createNodeRedirectOut(char* data, TreeNode* rightNode);
TreeNode* createNodeArgument(char *argument, TreeNode* tokenListNode);
TreeNode* createNodeCommand(char *command, TreeNode* tokenListNode);

void deleteNode(TreeNode* node);

#endif

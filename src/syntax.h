#ifndef SYNTAX_H
#define SYNTAX_H

enum {
    NODE_PIPE,
    NODE_REDIRECT_IN,
    NODE_REDIRECT_OUT,
    NODE_REDIRECT_OUT_APPEND,
    NODE_COMMAND,
    NODE_ARGUMENT,
};

typedef struct Node
{
    int type;
    char* szData;
    struct Node* left;
    struct Node* right;

} Node;

Node* createNodePipe(Node* leftNode, Node* rightNode);
Node* createNodeRedirectIn(char* data, Node* rightNode);
Node* createNodeRedirectOut(char* data, Node* rightNode);
Node* createNodeRedirectOutAppend(char* data, Node* rightNode);
Node* createNodeArgument(char *argument, Node* tokenListNode);
Node* createNodeCommand(char *command, Node* tokenListNode);

void deleteNode(Node* node);

#endif

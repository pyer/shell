#ifndef NODE_H
#define NODE_H

enum {
    NODE_PIPE,
    NODE_REDIRECT_IN,
    NODE_REDIRECT_OUT,
    NODE_REDIRECT_OUT_APPEND,
    NODE_COMMAND,
    NODE_ARGUMENT,
    NODE_VARIABLE,
};

typedef struct Node
{
    int type;
    char* szData;
    struct Node* left;
    struct Node* right;
} Node;

void deleteNode(Node* node);

#endif

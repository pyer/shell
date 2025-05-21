#ifndef VARIABLE_H
#define VARIABLE_H

typedef struct Variable
{
    char* name;
    char* value;
    struct Variable* next;
} Variable;

Variable* firstVariable = NULL;

void deleteVariables(void);

#endif

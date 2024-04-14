#ifndef BUILTINS_H
#define BUILTINS_H

#include <stdbool.h>

void free_memory(int argc, char** argv);
bool builtin_command(int argc, char** argv, int status);

#endif

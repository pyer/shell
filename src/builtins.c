
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <version.h>

/* Manage memory
 */
void free_memory(int argc, char** argv)
{
    for (int i = 0; i < argc; i++)
        free(argv[i]);
    free(argv);
}


/* Builtin commands
 */
void change_dir(int argc, char** argv)
{
    if (argc == 1) {
		    struct passwd *pw = getpwuid(getuid());
		    const char *homedir = pw->pw_dir;
		    chdir(homedir);
   } else if (argc == 2) {
        if (chdir(argv[1]) != 0) {
            perror(argv[1]);
        }
    } else {
        perror("cd: Too many arguments\n");
    }
}


/* Builtins
 */
bool builtin_command(int argc, char** argv, int status)
{
    char* command = argv[0];
    if (strcmp(command, "?") == 0) {
      printf("%d\n", status);
    } else if (strcmp(command, "cd") == 0) {
      change_dir(argc, argv);
    } else if (strcmp(command, "version") == 0) {
      printf("Version %s (%s %s)\n", VERSION, __DATE__, __TIME__);
    } else if (strcmp(command, "bye") == 0) {
      free_memory(argc, argv);
		  exit(0);
	  } else if (strcmp(command, "exit") == 0) {
      if (argc > 1) {
        status = atoi(argv[1]);
      }
      free_memory(argc, argv);
		  exit(status);
	  } else {
      return false;
    }
    return true;
}


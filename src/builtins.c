
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Manage memory
 */
void free_memory(int argc, char** argv)
{
    for (int i = 0; i < argc; i++)
        free(argv[i]);
    free(argv);
}

/* Builtin command
 */
bool builtin_command(int argc, char** argv)
{
    if (strcmp(argv[0], "cd") == 0) {
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
    } else if(strcmp(argv[0], "bye") == 0) {
      free_memory(argc, argv);
		  exit(0);
	  } else if(strcmp(argv[0], "exit") == 0) {
      free_memory(argc, argv);
		  exit(0);
	  } else {
      return false;
    }
    return true;
}


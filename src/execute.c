#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "builtins.h"
#include "node.h"
#include "signals.h"

/* code returned by the execvp process or builtin command */
int status = 0;

int last_status()
{
  return status;
}

void execute_simple_command(Node* simple_cmd_node,
                             bool stdin_pipe,
                             bool stdout_pipe,
                             int pipe_read,
                             int pipe_write,
                             char* redirect_in,
                             char* redirect_out,
                             bool append
                            )
{
    int argc;
    char** argv;

    if (simple_cmd_node == NULL)
      return;
    if (simple_cmd_node->type != NODE_COMMAND)
      return;

    // Init argc and argv
    Node* argNode = simple_cmd_node;
    int i = 0;
    while (argNode != NULL && (argNode->type == NODE_ARGUMENT || argNode->type == NODE_COMMAND)) {
        argNode = argNode->right;
        i++;
    }
    argv = (char**)malloc(sizeof(char*) * (i + 1));

    argNode = simple_cmd_node;
    i = 0;
    while (argNode != NULL && (argNode->type == NODE_ARGUMENT || argNode->type == NODE_COMMAND)) {
        argv[i] = (char*)malloc(strlen(argNode->szData) + 1);
        strcpy(argv[i], argNode->szData);
        argNode = argNode->right;
        i++;
    }
    argv[i] = NULL;
    argc = i;

    // check for built-in commands
    if (builtin_command(argc, argv, status)) {
      free_memory(argc, argv);
      return;
    }

    // Execute the command
    pid_t pid;
    if((pid = fork()) == 0 ) {
		    // restore the signals in the child process
        set_sigint_handler();
		    // store the stdout file desc
        int stdoutfd = dup(STDOUT_FILENO);

        // redirect stdin from file if specified
        if (redirect_in) {
            int fd = open(redirect_in, O_RDONLY);
            if (fd == -1) {
                perror(redirect_in);
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
        }
        // redirect stdout to file if specified
        if (redirect_out) {
            int flags = O_WRONLY | O_CREAT | O_TRUNC;
            if (append) {
                flags = O_WRONLY | O_CREAT | O_APPEND;
            }
            int fd = open(redirect_out, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fd == -1) {
                perror(redirect_out);
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
        }
        // read stdin from pipe if present
        if (stdin_pipe)
            dup2(pipe_read, STDIN_FILENO);
        // write stdout to pipe if present
        if (stdout_pipe)
            dup2(pipe_write, STDOUT_FILENO);

        if (execvp(argv[0], argv) == -1) {
			      // restore the stdout for displaying error message
            dup2(stdoutfd, STDOUT_FILENO);
            printf("Command not found: \'%s\'\n", argv[0]);
			      exit(1);
        }
    } else if (pid < 0) {
        perror("fork");
    } else {
        // wait till the process has not finished
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
          status = WEXITSTATUS(status);
        } else {
          status = 0;
        }
    }
    // Free allocated memory
    free_memory(argc, argv);
}

void execute_command(Node* cmdNode,
                      bool stdin_pipe,
                      bool stdout_pipe,
                      int pipe_read,
                      int pipe_write)
{
    if (cmdNode == NULL)
        return;

    switch (cmdNode->type)
    {
    case NODE_REDIRECT_IN:		// right side contains simple cmd node
        execute_simple_command(cmdNode->right,
                               stdin_pipe,
                               stdout_pipe,
                               pipe_read,
                               pipe_write,
                               cmdNode->szData, NULL,
                               false
                              );
        break;
    case NODE_REDIRECT_OUT:		// right side contains simple cmd node
        execute_simple_command(cmdNode->right,
                               stdin_pipe,
                               stdout_pipe,
                               pipe_read,
                               pipe_write,
                               NULL, cmdNode->szData,
                               false
                              );
        break;
    case NODE_REDIRECT_OUT_APPEND:
        execute_simple_command(cmdNode->right,
                               stdin_pipe,
                               stdout_pipe,
                               pipe_read,
                               pipe_write,
                               NULL, cmdNode->szData,
                               true
                              );
        break;
    case NODE_COMMAND:
        execute_simple_command(cmdNode,
                               stdin_pipe,
                               stdout_pipe,
                               pipe_read,
                               pipe_write,
                               NULL, NULL,
                               false
                              );
        break;
    }
}

void execute_pipeline(Node* t)
{
    int file_desc[2];

    pipe(file_desc);
    int pipewrite = file_desc[1];
    int piperead = file_desc[0];

	// read input from stdin for the first job
    execute_command(t->left, false, true, 0, pipewrite);
    Node* jobNode = t->right;

    while (jobNode != NULL && jobNode->type == NODE_PIPE)
    {
        close(pipewrite); // close the write end
        pipe(file_desc);
        pipewrite = file_desc[1];

        execute_command(jobNode->left, true, true, piperead, pipewrite);
        close(piperead);
        piperead = file_desc[0];

        jobNode = jobNode->right;
    }

    piperead = file_desc[0];
    close(pipewrite);
	
	// write output to stdout for the last job
    execute_command(jobNode, true, false, piperead, 0);	// only wait for the last command if necessary
    close(piperead);
}

void execute_syntax_tree(Node* t)
{
    if (t->type == NODE_PIPE) {
        execute_pipeline(t);
    } else {
        execute_command(t, false, false, 0, 0);
    }
}


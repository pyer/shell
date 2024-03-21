#include <stdbool.h>
#include <signal.h>

bool signalset = false;
void (*SIGINT_handler)(int);

// ignore some signals for the shell
void ignore_signals()
{
	signalset = true;
	// ignore "Ctrl-C"
    SIGINT_handler = signal(SIGINT, SIG_IGN);
	// ignore "Ctrl-Z"
    signal(SIGTSTP, SIG_IGN);
	// ignore "Ctrl-\"
    signal(SIGQUIT, SIG_IGN);
}

// restore Ctrl-C signal in the child process
void set_sigint_handler()
{
	if (signalset)
		signal(SIGINT, SIGINT_handler);
}


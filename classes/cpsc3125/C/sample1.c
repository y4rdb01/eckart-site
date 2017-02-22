/* This simple C program demonstrates the use of getpid, fork, wait, exit,
   and execv.  This is done illustrating how the command "ps -a" might be
   done from a parent "shell".
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
	int fork_rtn, child_status;
	printf("The parent pid is %d\n", (int) getpid());

	if (fork_rtn = fork() > 0) {
		/* This is the parent. */
		wait(&child_status);
	} else {
		/* This is the child. */
		printf("The child pid is %d\n", (int) getpid());

		fprintf(stderr, "Child is preparing to exec.\n");
		if (execv("/usr/bin/ps", (char *[]) { "ps", "-a", NULL }) == -1) {
			fprintf(stderr,
				"%s: Unable to execute 'ps'.\n", argv[0]);
			exit(3);
		}
	}

	printf("Here's the parent again (pid = %d).\n", (int) getpid());
	printf("The child had an exit status of %d\nDone.\n", child_status);
	return 0;
}

/* This simple C program demonstrates the use of getpid, fork, dup, dup2,
   wait, open, close, exit, chmod, and execv.  This is done illustrating how
   the command "tr Tt Zz < this > that" might be done from a parent "shell".
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	int fork_rtn, child_status;
	printf("The parent pid is %d\n", (int) getpid());

	if (fork_rtn = fork()) {
		/* This is the parent. */
		wait(&child_status);
	} else {
		/* This is the child. */
		int input_fd, output_fd;

		printf("The child pid is %d\n", (int) getpid());

		if ((input_fd = open("this", O_RDONLY)) >= 0) {
			close(0);
			dup2(input_fd, 0);
		} else {
			fprintf(stderr,
				"%s: Unable to open file 'this'.\n", argv[0]);
			exit(1);
		}

		if ((output_fd = creat("that", S_IRUSR | S_IWUSR)) >= 0) {
			close(1);
			dup2(output_fd, 1);
			/* Since my umask defaults to no permissions for
			   group or others, creat won't set them and thus
			   this is need to give group read permission.
			*/
			fchmod(output_fd, S_IRUSR | S_IWUSR | S_IRGRP);
		} else {
			fprintf(stderr,
				"%s: Unable to create file 'that'.\n", argv[0]);
			exit(2);
		}

		fprintf(stderr, "Child is preparing to exec.\n");
		if (execv("/usr/bin/tr", (char *[]) { "tr", "Tt", "Zz", NULL }) == -1) {
			fprintf(stderr,
				"%s: Unable to execute 'tr'.\n", argv[0]);
			exit(3);
		}
	}

	printf("Here's the parent again (pid = %d).\n", (int) getpid());
	printf("The child had an exit status of %d\nDone.\n", child_status);
	return 0;
}

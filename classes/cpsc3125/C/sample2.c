/* This simple C program demonstrates the use of pipes to allow communication
   between a parent and its child.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	int fork_rtn, child_status, tube[2];
	char c;

	printf("The parent pid is %d\n", (int) getpid());

	if (pipe(tube)) {
		fprintf(stderr, "%s: Unable to create pipe.\n", argv[0]);
		exit(1);
	}

	if (fork_rtn = fork()) {
		/* This is the parent. */

		close(tube[0]);
		printf("What character do you want to send to the child? ");
		c = getchar();
		write(tube[1], &c, 1);
		wait(&child_status);

		printf("Here's the parent again (pid = %d).\n", (int) getpid());
		printf("The child had an exit status of %d\nParent is done.\n",
			child_status);
	} else {
		/* This is the child. */

		close(tube[1]);
		printf("The child pid is %d\n", (int) getpid());
		read(tube[0], &c, 1);
		printf("The child got '%c'\n", c);
		printf("Sleeping child.\n");
		sleep(5);
		printf("Child (pid = %d) is done.\n", (int) getpid());
	}
	return 0;
}

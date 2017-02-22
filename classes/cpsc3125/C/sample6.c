/* This is a simple illustration of using pipes as a means for passing
   messages between processes. The parent and child pass a value back
   and forth, each taking the square root of the value before sending
   it back.
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <math.h>

int main(int argc, char *argv[]) {
        int parent2child_tube[2];
        int child2parent_tube[2];
	pid_t child;

        pipe(parent2child_tube);
        pipe(child2parent_tube);

        if(child = fork()){
		// Parent process

		// Parent output file descriptor
		close(parent2child_tube[0]);
		close(1);
        	dup2(parent2child_tube[1], 1);

		// Parent input file descriptor
		close(child2parent_tube[1]);
		close(0);
        	dup2(child2parent_tube[0], 0);

		// Send and receive messages until message value is <= 1.
		double value = 144;
		do {
			// Send value to child.
			fprintf(stderr, "Parent sending %lf to child.\n", value);
			printf("%lf\n", value);
			fflush(stdout);

			// Receive updated value from child.
			scanf("%lf", &value);
			fprintf(stderr, "Parent received %lf from child.\n", value);

			// Do computation.
			value = sqrt(value);
			
		} while (value > 1);

	} else {
		// Child process

		// Child output file descriptor
		close(child2parent_tube[0]);
		close(1);
        	dup2(child2parent_tube[1], 1);

		// Child input file descriptor
		close(parent2child_tube[1]);
		close(0);
        	dup2(parent2child_tube[0], 0);

		// Receive and send messages.
		while (1) {
			double value;

			// Receive value from parent.
			scanf("%lf", &value);
			fprintf(stderr, "Child received %lf from parent.\n", value);

			// Do computation.
			value = sqrt(value);
			
			// Send updated value back to parent.
			fprintf(stderr, "Child sending %lf to parent.\n", value);
			printf("%lf\n", value);
			fflush(stdout);
		}
	}

	// Parent is done, so kill the child rather than leave an orphan.
	kill(child, 9);
}

/* This reasonably simple C program is a test of the process system calls
   and the use of mutices.  NUM_PROCS are used to count upto (or just
   past) UPPER_LIMIT starting at LOWER_LIMIT.  Each process, however, will
   increase the shared counter by a different amount each time (resting after
   each addition).  Once the UPPER_LIMIT is reached (or surpassed) then the
   extra processes join up, the main/original process prints a message, and
   then quits.

   To get this to run correctly, you'll need to;
	1) Start a cygwin terminal (right click and run as administrator)
	2) Run "/usr/bin/cygserver-config" (answer "yes" to the questions)
	3) Run "/usr/bin/cygrunsrv -S cygserver"

*/
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h>

#define NUM_PROCS 3

#define LOWER_LIMIT 1
#define UPPER_LIMIT 25

#define SLEEPY_TIME 2

pid_t original_process;

int shm_id;

/* Organize the shared memory information and declare a pointer to it. */
typedef struct info {
	int counter;
	sem_t mutex;
} sharedInfo;

sharedInfo *shared;

/* Perform various cleanup operations. */
void cleanup() {
	/* Destroy the semaphores. */
	sem_destroy(&shared->mutex);

	/* Return the shared memory. */
	shmctl(shm_id, IPC_RMID, NULL);
}

/* Each process performs this operation as its only task. */
void process_loop(int counter_increment) {
	while (1) {
		int value;

		/* The counter can only be altered by one process at a time. */
		sem_wait(&shared->mutex);

		if (shared->counter >= UPPER_LIMIT) {
			sem_post(&shared->mutex);

			if (getpid() == original_process) break;
			else exit(0);
		}

		value = shared->counter += counter_increment;

		/* Allow another thread to change the counter. */
		sem_post(&shared->mutex);

		printf("Process %d increased counter by %d, now counter = %d\n",
			getpid(), counter_increment, value);

		sleep(SLEEPY_TIME);
	}
}

/* The main/driver process. */
int main(int argc, char *argv[]) {
	pid_t children[NUM_PROCS];
	int i;

	/* If something untoward happens, then try to cleanup. */
        signal(SIGHUP, cleanup);
        signal(SIGINT, cleanup);
        signal(SIGQUIT, cleanup);
        signal(SIGTERM, cleanup);

	/* Get the shared memory segment. */
	if ((shm_id = shmget(IPC_PRIVATE, sizeof(sharedInfo), 0600)) == -1) {
		perror("Unable to allocate shared memory segment.");
		exit(3);
	}

	/* Initialize the "shared" array. */
	shared = (sharedInfo *) shmat(shm_id, 0, 0600);
	sem_init(&(shared->mutex), 1, 1);
	shared->counter = LOWER_LIMIT;

	/* Remember which one you are. */
	original_process = getpid();

	/* Create NUM_PROCS-1 other processes. */
	for (i = 1; i < NUM_PROCS; i++) {
		children[i-1] = fork();

		if (children[i-1] < 0) {
			perror("Error while creating a process.");
			exit(3);
		} else if (children[i-1] == 0) {
			/* Child. */
			process_loop(1);
		}
	}

	/* Parent falls through the for loop. */
	process_loop(1);

	/* Join with the other processes. */
	for (i = 1; i < NUM_PROCS; i++) {
		int child_status;

		waitpid(children[i-1], &child_status, 1);
		printf("Exit status of process %d was %d\n",
			children[i-1], (int) child_status);
	}

	printf("All Done!\n");

	cleanup();

	return 0;
}

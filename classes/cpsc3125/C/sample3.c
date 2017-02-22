/* This moderately simple C program is a test of the shared memory operations
   and the use of semaphores with multiple processes.  The goal is to indicate
   to a group coordinator (master) when all of the subserviant (slave) processes
   have completed their subtask(s).  Thus there is strict alternation between
   the master and the group of slaves.  Furthermore a slave only does it's task
   ONCE between master actions.

   To get this to run correctly, you'll need to;
	1) Start a cygwin terminal (right click and run as administrator)
	2) Run "/usr/bin/cygserver-config" (answer "yes" to the questions)
	3) Run "/usr/bin/cygrunsrv -S cygserver"
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SLAVES 3

#define MAX_TIME 5

int kids[SLAVES], shm_id, master_sem_id, slave_sem_id;

/* Perform various cleanup operations. */
void cleanup() {

	/* Return the semaphores. */
	semctl(master_sem_id, SLAVES, IPC_RMID, 0);
	semctl(slave_sem_id, SLAVES, IPC_RMID, 0);

	/* Return the shared memory. */
	shmctl(shm_id, IPC_RMID, NULL);
}

struct sembuf down[SLAVES], up[SLAVES];

/* Each slave process performs this operation as their only task. */
void shared_loop(int pnumber, int ppid) {
	int i, *shared_addr = (int *) shmat(shm_id, 0, 0600);

	printf("slave %d (of parent with pid = %d) beginning.\n",
		pnumber, ppid);

	while (1) {
		/* Have the slave do its (sub)task. */
		semop(slave_sem_id, &down[pnumber], 1);

		/* Do slave (sub)task. */
		shared_addr[pnumber+1] = !shared_addr[pnumber+1];

		for (i = 0; i < SLAVES; i++)
			printf("slave %d: shared_addr[%d] = %d\n", 
				pnumber, i, shared_addr[i]);

		/* Allow the master to do its task. */
		semop(master_sem_id, &up[pnumber], 1);
	}
}

int main(int argc, char *argv[]) {
	int i, time, *shared_addr, ppid = getpid();

        signal(SIGHUP, cleanup);
        signal(SIGINT, cleanup);
        signal(SIGQUIT, cleanup);
        signal(SIGTERM, cleanup);

	/* Get the "master" semaphores. */
	if ((master_sem_id = semget(IPC_PRIVATE, SLAVES, 0600)) == -1) {
		perror("Unable to allocate semaphores for master.");
		exit(1);
	}

	/* Get the "slave" semaphores. */
	if ((slave_sem_id = semget(IPC_PRIVATE, SLAVES, 0600)) == -1) {
		perror("Unable to allocate semaphores for slave(s).");
		exit(2);
	}

	/* Build the "up" and "down" semaphore operation arrays. */
	for (i = 0; i < SLAVES; i++) {
		up[i].sem_num = down[i].sem_num = i;
		up[i].sem_op = 1;
		down[i].sem_op = -1;
		up[i].sem_flg = down[i].sem_flg = 0;
	}

	/* Have the "master" semaphores start off with a value of 1, while
	   the slave semaphores start off at 0.
	*/
	semop(master_sem_id, up, SLAVES);

	/* Get the shared memory segment. */
	if ((shm_id = shmget(IPC_PRIVATE, sizeof(int[SLAVES+1]), 0600)) == -1) {
		perror("Unable to allocate shared memory segment.");
		exit(3);
	}

	/* Initialize the "shared" array. */
	shared_addr = (int *) shmat(shm_id, 0, 0600);
	for (i = 0; i <= SLAVES; i++) shared_addr[i] = 1;

	/* Fork SLAVES other processes for a total of SLAVES+1 processes which
	   will share the variable "shared".
	*/
	for (i = 0; i < SLAVES; i++)
		if ((kids[i] = fork()) == 0)
			shared_loop(i, ppid);
		else if (kids[i] == -1) {
			perror("Error while forking.");
			exit(3);
		}

	/* The master process now coordinates the actions of the slaves. */
	for (time = 0; time < MAX_TIME; time++) {

		/* Have the master prepare to do its task(s). */
		semop(master_sem_id, down, SLAVES);

		/* Have the master do its thing. */
		printf("Master values at start of iteration %d:\n", time);
		for (i = 0; i <= SLAVES; i++)
			printf("    shared_addr[%d] = %d\n", i, shared_addr[i]);

		shared_addr[0] = !shared_addr[0];

		printf("Master values at end of iteration %d:\n", time);
		for (i = 0; i <= SLAVES; i++)
			printf("    shared_addr[%d] = %d\n", i, shared_addr[i]);

		/* Allow the slaves to do their subtask(s). */
		semop(slave_sem_id, up, SLAVES);
	}

	/* Kill the children since you are done. */
	for (i = 0; i < SLAVES; i++) kill(kids[i], SIGKILL);

	cleanup();

	return 0;
}

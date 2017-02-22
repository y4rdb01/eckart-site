/* This reasonably simple C program is a test of the POSIX thread operations
   and the use of mutices.  NUM_THREADS are used to count upto (or just
   past) UPPER_LIMIT starting at LOWER_LIMIT.  Each thread, however, will
   increase the shared counter by a different amount each time (resting after
   each addition).  Once the UPPER_LIMIT is reached (or surpassed) then the
   extra threads join up, the main/original thread prints a message, and
   the process quits.

   You might need to compile this program with:

		gcc -ansi -pedantic sample4.c -lpthread
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 3

#define LOWER_LIMIT 1
#define UPPER_LIMIT 25

#define SLEEPY_TIME 2

int shared_counter = LOWER_LIMIT;

pthread_mutex_t mutex;

pthread_t original_thread;

/* Each thread performs this operation as its only task. */
void *thread_loop(void *arg) {
	long int counter_increment = (long int) arg;

	while (1) {
		/* The counter can only be altered by one thread at a time. */
		pthread_mutex_lock(&mutex);

		if (shared_counter >= UPPER_LIMIT) {
			pthread_mutex_unlock(&mutex);

			if (pthread_self() == original_thread) break;
			else pthread_exit(0);
		}

		shared_counter += counter_increment;

		printf("Thread %d increased counter by %d, now counter = %d\n", 
			pthread_self(), counter_increment, shared_counter);

		/* Allow another thread to change the counter. */
		pthread_mutex_unlock(&mutex);

		sleep(SLEEPY_TIME);
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	pthread_t threads[NUM_THREADS];
	pthread_attr_t pthread_attr;
	long int i;

	pthread_mutex_init(&mutex, NULL);

	/* Remember which one you are. */
	original_thread = pthread_self();

	/* This gives threads that are scheduled on a system-wide basis. */
	pthread_attr_init(&pthread_attr);
	pthread_attr_setscope(&pthread_attr, PTHREAD_SCOPE_SYSTEM);

	/* Create NUM_THREADS-1 other threads. */
	for (i = 1; i < NUM_THREADS; i++)
		if (pthread_create(&threads[i-1], &pthread_attr,
				   thread_loop, (void *) (i + 1))) {

			perror("Error while creating a thread.");
			exit(3);
		}

	thread_loop((void *) 1);

	/* Join with the other threads. */
	for (i = 1; i < NUM_THREADS; i++) {
		void *thread_status;

		pthread_join(threads[i-1], &thread_status);
		printf("Exit status of thread %d was %ld\n",
			threads[i-1], (long int) thread_status);
	}

	printf("All Done!\n");

	pthread_mutex_destroy(&mutex);

	return 0;
}

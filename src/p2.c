#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <pthread.h>

// split is used to store the number of integers each thread has to handle
int split;

// array pointer is used to point to the shared memory location, and p stores the current location we are pointing to the shared memory
int *narray, *np;
FILE *fp;

// global totalSum variable to store the total sum of numbers
long long int totalSum = 0;

// mutex lock to synchronise
pthread_mutex_t nmutexLock;

void* calculate_sum(void *arg) {
	// worker function
	// used to calculate partial sum of the "split" amount of integers
	// totalSum is also computed by adding the total sum

	// Locking using pthread_mutex to avoid race conditions
	pthread_mutex_lock(&nmutexLock);
	long long int sum = 0;
	for (int i = 0; i < split; ++i) {
		sum += *np; // read the number from shared mem
		++np;// points to next location in shared mem
	}
	totalSum += sum;
	printf("\n Partial Sum = %lld\n", sum);
	pthread_mutex_unlock(&nmutexLock);
}

int main(int argc, char *argv[]) {

	// Used to clock the time
	clock_t time;
	time = clock();

	if (argc != 3) {
		printf("Usage: ./a.out num_of_integers inputfile");
		exit(-1);
	}

	// [ATTENTION]: Update the number of threads as required
	long long int number_of_threads = 5;
	long long int size = atoi(argv[1]);
	fp = fopen(argv[2], "r");

	int number_of_threads = atoi(argv[3]);
	split = size / number_of_threads;

	pthread_t sth[number_of_threads];
	key_t key;

	// Request a key
	// We are mapping the key to the test_data file
	key = ftok(argv[2], 65);


	int shared_block_id = shmget(key, size * sizeof(int), IPC_CREAT | 0666);

	// map the shared block into this process's memory
	// and return a pointer to it
	narray = shmat(shared_block_id, (void*)0, 0);

	// mapping the first pointer of array to global pointer used by threads that is incremented
	np = narray;

	// The "pthread create and join part"
	for (int i = 0; i < number_of_threads; ++i) {
		if (pthread_create(&sth[i], NULL, &calculate_sum, NULL) != 0) {
			perror("Failed to create the file");
		}
	}

	for (int i = 0; i < number_of_threads; ++i) {

		if (pthread_join(sth[i], NULL) != 0) {
			perror("Failed to create the file");
		}
	}

	// Printing the totalSum computed after all threads are done with their work
	printf("\nTotal Sum: %lld\n", totalSum);

	time = clock() - time;
	double time_taken = ((double)time) / CLOCKS_PER_SEC;
	printf("[log]: Code took %f seconds to execute\n", time_taken);

	shmdt(narray);
}
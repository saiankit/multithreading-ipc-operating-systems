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
int *array, *p;
FILE *fp;

pthread_mutex_t mutexLock;

void* read_file(void *arg) {
	// worker function
	// used to read the file ( but only reading "split" amount of integers)
	// All the worker functions read "split" amount of integers,
	// and in total read the whole file

	// Locking using pthread_mutex to avoid race conditions
	pthread_mutex_lock(&mutexLock);
	for (int i = 0; i < split; ++i) {
		int a;
		fscanf(fp, "%d", &a);
		*p = a; // store the number in shared mem
		printf("%d ", *p);
		++p; // points to next location in shared mem
	}
	pthread_mutex_unlock(&mutexLock);
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

	pthread_t th[number_of_threads];
	key_t key;

	// Request a key
	// We are mapping the key to the test_data file
	key = ftok(argv[2], 65);

	int shared_block_id = shmget(key, size * sizeof(int), IPC_CREAT | 0666);

	// map the shared block into this process's memory
	// and return a pointer to it
	array = shmat(shared_block_id, (void*)0, 0);

	// mapping the first pointer of array to global pointer used by threads that is incremented
	p = array;

	// The "pthread create and join part"
	for (int i = 0; i < number_of_threads; ++i) {
		if (pthread_create(&th[i], NULL, &read_file, NULL) != 0) {
			perror("Failed to create the file");
		}
	}

	for (int i = 0; i < number_of_threads; ++i) {

		if (pthread_join(th[i], NULL) != 0) {
			perror("Failed to create the file");
		}
	}

	time = clock() - time;
	double time_taken = ((double)time) / CLOCKS_PER_SEC;
	printf("[log]: Code took %f seconds to execute\n", time_taken);

	shmdt(array);
}
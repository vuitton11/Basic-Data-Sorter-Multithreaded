/*
 * counter.h - threads can access the values of other threads. Therefore, variables must
 * me editted with mutexes inside structs to ensure only 1 thread can access it at a time.
 *
 * This contains all methods to safely modify the thread counter and the database counter
 */
#include <pthread.h>

//This struct holds the counter that can be editted by other threads,
//but is prevented via mutex
typedef struct _counter {
    int genericCount;
    pthread_mutex_t lock;
} counter;

//Declare functions
void createCounter(counter *temp);
void incrementCounter(counter *temp);
int getCounter(counter *temp);

void createCounter(counter *temp) {
	temp->genericCount = 0;
	pthread_mutex_init(&temp->lock, NULL);
}

void incrementCounter(counter *temp) {
	int tempCounter;

	pthread_mutex_lock(&temp->lock);
	
	tempCounter = temp->genericCount;
	tempCounter++;
	temp->genericCount = tempCounter;

	pthread_mutex_unlock(&temp->lock);
}

int getCounter(counter *temp) {
	int tempCounter;

	pthread_mutex_lock(&temp->lock);

	tempCounter = temp->genericCount;

	pthread_mutex_unlock(&temp->lock);

	return tempCounter;
}


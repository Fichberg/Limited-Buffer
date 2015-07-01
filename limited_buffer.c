#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define BUFSIZE      10
#define PRODUCER     'P'
#define CONSUMER     'C'
#define PRODUCERS    10
#define CONSUMERS    10
#define STOP_COND    10000

typedef struct thread {
	int number;
	char type;
	int data;
} Thread;

int rear = 0, front = 0, count = 0, data = 0, buf[BUFSIZE];
sem_t mutex, mutex1, mutex2;

void *run(void*);
void initiate_threads(Thread*);
void initiate_semaphores();
void destroy_semaphores();

int main(int argc, char **argv)
{
	int i;
	pthread_t *threads;
	Thread *args;

	threads = malloc((PRODUCERS + CONSUMERS) * sizeof(*args));
	args = malloc((PRODUCERS + CONSUMERS) * sizeof(*args));

	initiate_threads(args);
	initiate_semaphores();

	for(i = 0; i <= PRODUCERS + CONSUMERS; i++)
	{
		int j = i + 1;
		if(j > PRODUCERS) j -= PRODUCERS;

		if (pthread_create(&threads[i], NULL, run, &args[i]))
		{
			printf("Error creating thread.");
			abort();
		}
	}
	for(i = 0; i <= PRODUCERS + CONSUMERS; i++)
	{
		if (pthread_join(threads[i], NULL))
		{
			printf("Error joining thread.");
			abort();
		}
	}

	free(args); args = NULL;
	free(threads); threads = NULL;
	destroy_semaphores();
	return 0;
}

void *run(void *args)
{
	Thread *thread = ((Thread*) args);

	if(thread->type == PRODUCER)
	{
		while(1)
		{
			while(count >= BUFSIZE) sem_wait(&mutex1);
			sem_wait(&mutex);
			buf[rear] = data++;
			printf("Count(%d): PRODUCER #%d: %d deposited in the buffer.\n", count, thread->number, buf[rear]);
			rear = (rear + 1) % BUFSIZE;
			count++;
			if(count == 1) sem_post(&mutex2);
			sem_post(&mutex);
		}
	}
	if(thread->type == CONSUMER)
	{
		while(1)
		{
			while(count <= 0) sem_wait(&mutex2);
			sem_wait(&mutex);
			thread->data = buf[front];
			printf("Count(%d): CONSUMER #%d: %d read from the buffer.\n", count, thread->number, thread->data);
			front = (front + 1) % BUFSIZE;
			count--;
			if(count == BUFSIZE - 1) sem_post(&mutex1);
			sem_post(&mutex);
		}
	}

	return NULL;
}

void initiate_threads(Thread *args)
{
	int i;

	for(i = 0; i < PRODUCERS + CONSUMERS; i++)
	{
		int j = i + 1;
		if(j > CONSUMERS) {
			j -= CONSUMERS;
			args[i].number = j;
			args[i].type = PRODUCER;
			args[i].data = 0;
		}
		else {
			args[i].number = j;
			args[i].type = CONSUMER;
			args[i].data = 0;
		}
	}
}

void initiate_semaphores()
{
	if(sem_init(&mutex, 0, 1))
	{
		printf("Error creating semaphore 0.\n");
		exit(-1);
	}
	if(sem_init(&mutex1, 0, 1))
	{
		printf("Error creating semaphore 1.\n");
		exit(-1);
	}
	if(sem_init(&mutex2, 0, 1))
	{
		printf("Error creating semaphore 2.\n");
		exit(-1);
	}
	sem_wait(&mutex2);
}

void destroy_semaphores()
{
	sem_destroy(&mutex);
	sem_destroy(&mutex1);
	sem_destroy(&mutex2);
}
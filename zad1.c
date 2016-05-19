#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define PHILOSOPHER_AMOUNT 5

pthread_t freds[PHILOSOPHER_AMOUNT];
sem_t forks[PHILOSOPHER_AMOUNT];
int createNext=1;
void thinking(int id);
void eating(int id);
void *run(void *arg);
void v(int id);
void p(int id);

int main()
{
	for (int i = 0; i < PHILOSOPHER_AMOUNT; i++)
	{
		sem_init(&(forks[i]),0,1);
	}
	for (int i = 0; i < PHILOSOPHER_AMOUNT; i++)
	{
		pthread_create(&(freds[i]),NULL,&run,&i);
		createNext=0;
		while(!createNext);
	}
	for (int i = 0; i < PHILOSOPHER_AMOUNT; i++)
	{
		pthread_join(freds[i],NULL);
	}

	return 0;
}
void *run(void *arg)
{
	createNext=1;
	int id=*((int*)arg);
	int firstFork=(id == 0 ? 0 : id-1);
	int secondFork=(id == 0 ? PHILOSOPHER_AMOUNT-1 : id);
	while(1)
	{
		thinking(id);
		v(firstFork);
		v(secondFork);
		eating(id);
		p(secondFork);
		p(firstFork);
		sleep(1);
	}
}

void eating(int id)
{
	printf("%d is eating\n", id);
}
void thinking(int id)
{
	printf("%d is thinking\n", id);
}
void v(int id)
{
	sem_wait(&forks[id]);
}
void p(int id)
{
	sem_post(&forks[id]);	
}


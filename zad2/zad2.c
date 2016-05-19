#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define FLYING 3
#define STAYING 2

pthread_mutex_t carrier; 
pthread_cond_t toStart;
pthread_cond_t toLand;
int onCarrier=0;
int isFree=1;
int n;
int k;
int planesAmount;
int amountPlanesToLand=0;
int amountPlanesToStart=0;
pthread_t *planes;

void freeAirstrip();
void agreementToStart(int id);
void agreementToLand(int id);
void start(int id);
void land(int id);
void fly(int id);
void stop(int id);
void *run(void *arg);
int main(int argc, char *argv[])
{
	if(argc!=4)
	{
		printf("Examlple start up: ./zad2 [N] [K] [Planes Amount]\n");
		return -1;
	}
	n=atoi(argv[1]);
	k=atoi(argv[2]);
	if(k>n)
	{
		printf("k must not be greater than n!\n");
		return -1;
	}
	planesAmount=atoi(argv[3]);
	planes=(pthread_t*)malloc(planesAmount*sizeof(pthread_t));
	pthread_mutex_init(&carrier, NULL);
    pthread_cond_init (&toStart, NULL);
    pthread_cond_init (&toLand, NULL);
	for (int i = 0; i < planesAmount; i++)
	{
		pthread_create(&(planes[i]),NULL,&run,&i);
	}
	for (int i = 0; i < planesAmount; i++)
	{
		pthread_join(planes[i],NULL);
	}
	return 0;
}
void *run(void *arg)
{
	int id=*((int*)arg);
	while(1)
	{
		agreementToLand(id);
		land(id);
		sleep(STAYING);
		stop(id);
		agreementToStart(id);
		start(id);
		sleep(STAYING);
		fly(id);
	}
}
void freeAirstrip()
{
	if(onCarrier<k)
	{
		if(amountPlanesToLand>0)
		{
			if(pthread_cond_signal(&toLand) < 0) 
			{
        		printf("landing conditions signal failed");
        		exit(0);
    		}
		}
		else
		{
			if(pthread_cond_signal(&toStart) < 0) 
			{
        		printf("starting conditions signal failed");
        		exit(0);
    		}
		}
	}
	else
	{
		if(amountPlanesToStart>0)
		{
			if(pthread_cond_signal(&toStart) < 0) 
			{
		        printf("startinconditions signal failed");
		        exit(0);
    		}
		}
		else
		{
		    if(pthread_cond_signal(&toLand) < 0) 
		    {
    		    printf("landing conditions signal failed");
        		exit(0);
    		}
		}
	}
}
void agreementToStart(int id)
{
	pthread_mutex_lock(&carrier);
	printf("Plane %d is waiting for start\n",id);
	amountPlanesToStart++;
	while(isFree==0)
	{
		if(pthread_cond_wait(&toStart, &carrier) < 0) 
		{
	        printf("start conditions wait failed");
	        exit(0);
    	}
	}
	isFree=0;
	amountPlanesToStart--;
	pthread_mutex_unlock(&carrier);
}
void agreementToLand(int id)
{
	printf("Plane %d is waiting for landing\n",id);
	pthread_mutex_lock(&carrier);
	amountPlanesToLand++;
	while(isFree==0 || onCarrier == n)
	{
		if(pthread_cond_wait(&toLand, &carrier) < 0)
		{
	        printf("landing conditions wait failed");
	        exit(0);
    	}
	}
	isFree=0;
	amountPlanesToLand--;
	pthread_mutex_unlock(&carrier);
}
void start(int id)
{
	pthread_mutex_lock(&carrier);
	onCarrier--;
	isFree=1;
	freeAirstrip();
	printf("Plane %d has just started\n", id);
	pthread_mutex_unlock(&carrier);
}
void land(int id)
{
	pthread_mutex_lock(&carrier);
	onCarrier++;
	isFree=1;
	freeAirstrip();
	printf("Plane %d has just landed\n", id);
	pthread_mutex_unlock(&carrier);
}
void fly(int id)
{
	printf("Plane %d is flying\n", id);
	sleep(FLYING);
}
void stop(int id)
{
	printf("Plane %d is resting\n", id);
	sleep(STAYING);
}

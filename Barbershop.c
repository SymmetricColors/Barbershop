#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#define MAX_customers 10
#define NUM_customers 50
int customers=0;
int n_filmfare=4;
int value;
pthread_t cust[NUM_customers];
pthread_t barb[3];
sem_t mutex;
sem_t chair;
sem_t barber;
sem_t customer;
sem_t cash;
sem_t receipt;
sem_t mutex1;

typedef struct
{
	sem_t went;//completed one
	sem_t left;//remaining one
}queue;
queue *waiting_room,*filmfare,*os;
queue* create_queue(int num)
{
	queue* q=(queue*)malloc(sizeof(queue));
	sem_init(&(q->went),0,0);
	sem_init(&(q->left),0,num);
	return q;
}
int wait_queue(queue* q,int n)
{
	sem_getvalue(&q->left,&value);
	sem_wait(&(q->left));
	sem_post(&(q->went));
	return value;
}
void* cutting(void* arg)
{
	int n=*(int *)arg;
	while(1)
	{
		sem_wait(&customer);
		sem_post(&barber);
		printf("\t barber %d is cutting hair\n",n);
		sleep(3);
		sem_wait(&cash);
		printf("\t barber %d is accepting payment\n",n);
		sleep(1);
		sem_post(&receipt);
		sem_post(&chair);
	}
}
void signal_queue(queue *q){

	sem_wait(&(q->went));
	sem_post(&(q->left));
}
void* barbershop(void *arg)
{
	int n=*(int *)arg;
	int left_f;
	sem_wait(&mutex); 
	if(customers>=MAX_customers)
	{
		sem_post(&mutex);
		printf("No space for more customers,exiting shop %d..\n",n);
	}
	customers=customers+1;
	sem_post(&mutex);
	wait_queue(waiting_room,n);
	printf("Entering in to waiting_room %d\n",n);
	if(wait_queue(filmfare,n)==0)
	{
		printf("%d Reading os book\n",n);
	}
	printf("%d reading filmfare\n",n);
	signal_queue(waiting_room);
	sem_wait(&chair); //acquire chair
	printf("%d is on barber chair for haircut\n",n);
	sleep(3);
	signal_queue(filmfare);
	sem_post(&customer);
	sem_wait(&barber);
	printf("\t %d get hair cut\n ",n);
	sleep(2);
	printf("\t\t customer %d got the hair cut and ready to pay\n",n);
	sem_post(&cash);
	sem_wait(&receipt);
	sem_wait(&mutex); //lock for changig customer only one thread will change the customer value at a time
	customers=customers-1;
	sem_post(&mutex);
	printf("customer %d exiting shop....\n",n);
}
int main()
{
	int i,j,cust_id[NUM_customers],barber_id[3];
	char ch;
	sem_init(&mutex,0,1);
	sem_init(&chair,0,3);
	sem_init(&barber,0,0);
	sem_init(&customer,0,0);
	sem_init(&cash,0,0);
	sem_init(&receipt,0,0);
	waiting_room=create_queue(6);
	filmfare=create_queue(4);
	os=create_queue(50);
	for(i=0;i<3;i++)
	{
		//barb[i]=i;
		barber_id[i]=i;
		pthread_create(&barb[i],0,cutting,&barber_id[i]);
		printf("barber %d is sleeping\n",i);
	}
	printf("Enter c for customer and e to exit\n");
	j=0;
	while(1)
	{
		scanf("%c",&ch);
		if(ch=='c')
		{
			cust_id[j]=j;
			pthread_create(&cust[i],0,barbershop,&cust_id[j]);
			j++;
		}
		if(ch=='e')
			{
				return(0);
			}
	}
	while(1);
}

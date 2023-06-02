#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

int counter = 0,counter_lock=0,loop=1000;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* increment(){
	for(int i=0;i<loop;i++) 	counter++;
}

void* increment_lock(){
	pthread_mutex_lock(&lock);
	for(int i=0;i<loop;i++)	counter_lock++;
	pthread_mutex_unlock(&lock);
}

int main(){
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_t p[10];
	pthread_t pl[10];
	for(int i=0;i<10;i++)	pthread_create(&p[i],NULL,increment,NULL);
	for(int i=0;i<10;i++)	pthread_join(p[i],NULL);

	pthread_mutex_lock(&mutex);
	for(int i=0;i<10;i++)	pthread_create(&pl[i],NULL,increment_lock,NULL);
	for(int i=0;i<10;i++)	pthread_join(pl[i],NULL);
	pthread_mutex_destroy(&mutex);

	printf("counter without lock %d\n",counter);
	printf("counter with lock %d\n",counter_lock);
}

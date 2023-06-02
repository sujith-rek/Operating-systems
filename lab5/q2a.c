#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int threads;
volatile int cnt = 0;

void* print_thread(void* arg){
	int turn = *(int*)arg;
	while (1) 
    	printf("%d ", turn + 1);			
	return NULL;
}

int main(){
	pthread_t* tid;
	volatile int i;
	int* arr;

	printf("Enter number of threads: ");
	scanf("%d", &threads);
	tid = (pthread_t*)malloc(sizeof(pthread_t) * threads);
	arr = (int*)malloc(sizeof(int) * threads);

	for (i = 0; i < threads; i++) {
		arr[i] = i;
		pthread_create(&tid[i], NULL, print_thread, (void*)&arr[i]);
	}

	for (i = 0; i < threads; i++) 	pthread_join(tid[i], NULL);
}


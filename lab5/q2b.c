#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t* cond = NULL;

int threads,cnt = 0;

void* print_thread(void* arg){
	int turn = *(int*)arg;

	while(1){
		pthread_mutex_lock(&mutex);

		if(turn != cnt) 	pthread_cond_wait(&cond[turn], &mutex);

		printf("%d ", turn + 1);

		if(cnt < threads - 1) 	cnt++;
		else 	cnt = 0;
		
		pthread_cond_signal(&cond[cnt]);
		pthread_mutex_unlock(&mutex);
	}

	return NULL;
}

int main(){
	pthread_t* tid;
	int* arr;

	printf("Enter number of threads: ");
	scanf("%d", &threads);

	cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t) * threads);
	tid = (pthread_t*)malloc(sizeof(pthread_t) * threads);
	arr = (int*)malloc(sizeof(int) * threads);

	for(int i=0;i<threads;i++){
		if(pthread_cond_init(&cond[i], NULL) != 0) {
			perror("pthread_cond_init() error");
			exit(1);
		}
	}

	for(int i=0;i<threads;i++){
		arr[i] = i;
		pthread_create(&tid[i], NULL, print_thread, (void*)&arr[i]);
	}

	for(int i=0;i<threads;i++) 	pthread_join(tid[i], NULL);
	
}
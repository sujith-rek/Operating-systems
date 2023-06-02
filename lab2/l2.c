#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <wait.h>
void *p1(){
	for(int i=0;i<5;i++){
	//sleep(1);
	printf("Process 1\n");}
	return NULL;
}
void *p2(){
	for(int i=0;i<8;i++){
	//sleep(1);
	printf("Process 2\n");}
	return NULL;
}

int main(){
char  line[MAX_INPUT_SIZE];            
	pthread_t thread_id[len];
	char **command = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	int ind = 0,i=0;
	while(i<n){
		int j = 0;
		while(tokens[i]!=NULL && strcmp(tokens[i],"&&&")){
			command[j]=tokens[i];
			j++;
			i++;
		}
		command[j]=NULL;
		i++;
		pthread_create(&thread_id[ind], NULL, processCommand,command);
		pthread_join(thread_id[ind], NULL);
		ind++;
	}    
}
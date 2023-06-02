#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

FILE *fg,*bg;

pid_t background[64],foreground[64];
int ind=0,pfg=0,sfg=0,be=0,pg;

// Splits the string by space and returns the array of tokens
char **tokenize(char *line){
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
      token[tokenIndex] = '\0';
      if (tokenIndex != 0){
	tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
	strcpy(tokens[tokenNo++], token);
	tokenIndex = 0; 
      }
    } else  token[tokenIndex++] = readChar;
  }
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}

//calculate number of tokens
int arrayLen(char **tokens){
	int n = 0;
	for(int i=0;tokens[i]!=NULL;i++) {
		n++;
		if(!strcmp(tokens[i],"&")) be=1;
		if(!strcmp(tokens[i],"&&")) sfg=1;
		if(!strcmp(tokens[i],"&&&")) pfg++;
	}
	return n;
}

//Execution of commands
int processCommand(char **tokens){
	pid_t pid = fork();
	if(pid==0) {
		execvp(*tokens, tokens);
		exit(0);
	}
	return pid;
}

//Removing the terminated background process from list
void delAndShift(pid_t p){
	for(int i=0;i<ind;i++){
		if(background[i]==p){
			for(int j=i;j<ind-1;j++){
				background[j] = background[j+1];
			}
			background[ind] = -1;
			ind--;
			return;
		}
	}	
	return ;
}

//Ctrl-C signal handler
void handle(int sig){
	if(foreground[0]!=0){
		printf(" KILLING process %d \n",pg);
		kill(foreground[0],SIGINT);
		printf("\n");
	}
	return ;
}

//Kills all background processes
void killAll(int n){
	for(int i=0;i<n;i++) kill(background[i],SIGINT);
}

//Reaping finished background procecsses
void back(int signum){
	pid_t pid;
    	while ((pid = waitpid(-1, NULL, WNOHANG)) > 0){
    		fprintf(bg,"killed %d\n",pid);
        	printf("Shell : Background process ended \n");
        	delAndShift(pid);
	}
}

//Serial Foreground execution
void serialFgExec(char **tokens,int n){
	char **command = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	for(int i=0;i<n;){
		int j = 0;
		while(tokens[i]!=NULL && strcmp(tokens[i],"&&")){
			command[j]=tokens[i];
			j++;
			i++;
		}
		command[j]=NULL;
		i++;
		processCommand(command);
		wait(NULL);
	}
}

//Parallel Foreground Execution
void parallelFgExec(char**tokens,int n,int len){
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

int main(int argc, char* argv[]) {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;
	char s[MAX_INPUT_SIZE];     
	int i;
	//==========================
	fg = fopen("fg.txt","w");
	bg = fopen("bg.txt","w");
	//==========================
	signal(SIGINT,handle);
	signal(SIGCHLD,back);
	
	while(1) {			
		/* BEGIN: TAKING INPUT */
		bzero(line, sizeof(line));
		printf("%s$ ",getcwd(s,MAX_INPUT_SIZE));
		scanf("%[^\n]", line);
		getchar();
		/* END: TAKING INPUT */
		line[strlen(line)] = '\n'; //terminate with new line
		
		tokens = tokenize(line);
		if(*tokens==NULL) continue;
		int size = arrayLen(tokens);
		//Implementation of cd command
       		if(!strcmp(tokens[0],"cd")){
       			if(size > 2) printf("cd: too many arguments \n");
       			else{
		       		int f = chdir(tokens[1]);
		       		if(f!=0) printf("cd: no such file or directory: %s\n",tokens[1]);
			}
				
		}
		//Exit
		else if(strcmp(tokens[0],"exit")==0){
			if(size==1) break;
			else printf("Too many arguments for exit");
		}
		//Serial foreground execution
		else if(sfg==1){
			if(pfg!=0 || be==1){ 
				printf("Error : && can't be used with other types of executions\n");
				pfg=0;	
				be=0;
			}else{
				serialFgExec(tokens,size);
			}
			sfg=0;
		}
		//Parallel foreground execution
		else if(pfg!=0){
			if(be==1){
				printf("Error : & can't be used with &&&");
				be=0;
			}else{
				parallelFgExec(tokens,size,pfg);
			}
			pfg = 0;
		}
		//Background process
		else if(strcmp(tokens[size-1],"&")==0){
			if(ind==64){
				printf("Maximum background processes limit reached \n");
				continue;
			}
			tokens[size-1] = NULL;
			
			background[ind]=processCommand(tokens);
			setpgid(background[ind],background[ind]);
			printf("[1] %d\n",background[ind]);
			fprintf(bg,"%d pid\nbg process %s\n\n",background[ind],tokens[0]);
			ind++;
		}
		//Foreground process
		else{
			pid_t child = processCommand(tokens);
			setpgid(child,child);
			foreground[0]=child;
			wait(NULL);
		}
		// Freeing the allocated memory	
		for(i=0;tokens[i]!=NULL;i++)	free(tokens[i]);
		free(tokens);
	}
	//Kill all the bakcground processes
	killAll(ind);
	fprintf(fg,"\n==========>End of Shell<==========\n");
	fprintf(bg,"\n==========>End of Shell<==========\n");
	fclose(fg);
	fclose(bg);
	return 0;
}
/*
=============================> Problems <===================================
1. termination of background process in the middle of foreground process returns prompt but it's still running
2. Ctrl-C signal should stop all the serial foreground executions
3. Parallel foreground execution
4. Should group foreground processes and background processes using setpgid

*/
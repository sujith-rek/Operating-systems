/*
=================================> First Come First Serve <=================================
*/
#include<stdio.h>
#include<stdlib.h>

struct process{
	int pid;	//process id
	int bTime;	//burst time
	int tat;	//turn around time
};

struct queue{
	struct process *p;
	int arTime;	//arrival time
	int waTime;	//waiting time
	struct queue *next;
};

int clock=0;

struct queue * enqueue(struct queue * head,int at,int bt,int pi){
	struct process * np = (struct process*)malloc(sizeof(struct process *));
	np->pid=pi;
	np->bTime=bt;
	np->tat=0;
	struct queue * nq = (struct queue *)malloc(sizeof(struct queue *));
	nq->p=np;
	nq->arTime=at;
	nq->waTime=0;
	nq->next=NULL;
	if(head==NULL) head=nq;
	else{
		struct queue * temp=head;
		while(temp->next!=NULL) temp=temp->next;
		temp->next=nq;
	}
	return head;
}

void printGant(struct queue * head,int n){
	if(head==NULL) {
		printf("Empty Queue");
		return ;
	}
	struct queue * temp=head;
	int arr[clock],totalWTime=0,totaltat=0;
	for(int i=0;i<clock;i++) arr[i]=0;
	printf("PID\tTurnaround time \tWaiting time\tTime interval\n");
	while(temp!=NULL){
		int Pid = temp->p->pid;
		int TAT = temp->p->tat;
		int WAT = temp->waTime;
		int AT = temp->arTime;
		int ct = TAT + AT;
		totaltat+=TAT;
		totalWTime+=WAT;
		printf("%d\t\t%d\t\t%d\t\t(%d - %d)\n",Pid,TAT,WAT,AT+WAT,ct);
		arr[ct-1] = ct; 
		temp=temp->next;
	}
	printf("\n");
	for(int i=0;i<clock;i++){
		if(arr[i]==0) printf("-");
		else printf("-|");
	}
	printf("\n");
	for(int i=0;i<clock;i++){
		if(arr[i]!=0) printf(" %d",arr[i]);
		else printf(" ");
	}
	printf("\n\nAverage waiting time = %.2f\nAverage Turnaround time = %.2f",(float)totalWTime/n,(float)totaltat/n);
	return ;
}

struct queue * deQ(struct queue * head){
	if(head==NULL) return NULL;
	struct queue * temp = head;
	head=head->next;
	temp->next=NULL;
	return head;
}

struct queue * enQ(struct queue * head, struct queue * node){
	if(node==NULL) return head;
	if(head==NULL) head=node;
	else{
		struct queue * temp = head;
		while(temp->next!=NULL) temp=temp->next;
		temp->next=node; 
	}
	return head;
}

void execute(struct queue * head,int n){
	struct queue * readyQ = NULL;
	struct queue * doneQ = NULL;
	
	while(head!=NULL && head->arTime<=clock ){
		readyQ = enQ(readyQ,head);
		head = deQ(head);
	}
	while(readyQ!=NULL || head!=NULL){
		if(readyQ!=NULL){
			struct queue * temp = readyQ;
			int bt = temp->p->bTime;
			temp->waTime = clock - (temp->arTime);
			temp->p->tat = (temp->waTime);
			while(bt!=0){
				(temp->p->tat) += 1;
				bt--;
				clock++;
				while(head!=NULL && head->arTime<=clock ){
					readyQ = enQ(readyQ,head);
					head = deQ(head);
				}
			}
			doneQ = enQ(doneQ,readyQ);
			readyQ = deQ(readyQ);
		}
		else{
			clock++;
			while(head!=NULL && head->arTime<=clock ){
				readyQ = enQ(readyQ,head);
				head = deQ(head);
			}
		}
	}
	printGant(doneQ,n);
}

struct queue * swap (struct queue * q1, struct queue * q2){
	struct queue * temp = q2->next;
	q2->next = q1;
	q1->next = temp;
	return q2;
}

void sort(struct queue ** head,int n){
	if(n==1) return ;
	for(int i=0;i<n;i++){
		struct queue ** temp = head;
		for(int j=0;j<n-1;j++){
			struct queue * q1 = *temp;
			struct queue * q2 = q1->next;
			if((q1->arTime)>(q2->arTime)){
				*temp = swap(q1,q2);
			}
			temp = &(*temp)->next;
		}	
	}
	return ;
}

int main(){
    FILE *ip;
    ip=fopen("input.txt","r");
    if(ip==NULL){
    	printf("Input file doesnt exist");
    	exit(0);
    }
    struct queue * jobQ = NULL;
    //Number of processes
    int n;
    fscanf(ip,"%d",&n);
    int p,a,b;
    for(int i=0;i<n;i++){
    	fscanf(ip,"%d%d%d",&p,&a,&b);
    	jobQ=enqueue(jobQ,a,b,p);
    }    
    fclose(ip);
    sort(&jobQ,n);
    execute(jobQ,n);
    free(jobQ);
}

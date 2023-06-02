/*
==================================> Round Robin <==================================

Assumptions:
if a process arrives to queue and the current running process need to be pushed 
to the back of queue at the same time
first we will queue current runnning process and then enqueue the incoming process
*/
#include<stdio.h>
#include<stdlib.h>

struct process{
	int pid;	//process id
	int bTime;	//burst time
	int wt;		//waiting time
	int tat;	//turn around time
};

struct queue{
	struct process *p;
	int arTime;	//arrival time
	int ind;
	int time[100];
	struct queue *next;
};

int clock= 1;

struct queue * enqueue(struct queue * head,int at,int bt,int pi){
	struct process * np = (struct process*)malloc(sizeof(struct process *));
	np->pid=pi;
	np->bTime=bt;
	np->wt=0;
	np->tat=0;
	struct queue * nq = (struct queue *)malloc(sizeof(struct queue *));
	nq->p=np;
	nq->arTime=at;
	nq->ind=0;
	nq->next=NULL;
	if(head==NULL) head=nq;
	else{
		struct queue * temp=head;
		while(temp->next!=NULL) temp=temp->next;
		temp->next=nq;
	}
	return head;
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

void printGant(struct queue * head,int arr[],int n,int p){
	if(head==NULL) {
		printf("Empty queue \n");
		return ;
	}
	int tott=0,totw=0;
	
	struct queue * temp = head;
	printf("PID\tTurnaround time \tWaiting time\tTime interval\n");
	//printf("PID\tTurnaround time \tWaiting time\n");
	while(temp!=NULL){
		printf("%d\t\t%d\t\t%d\t\t",temp->p->pid,temp->p->tat,temp->p->wt);
		int index = temp->ind;
		for(int i=0;i<index;i+=2){
			printf("(%d - %d) ",temp->time[i],temp->time[i+1]);
		}
		tott+=(temp->p->tat);
		totw+=(temp->p->wt);
		printf("\n");
		temp=temp->next;
	}
	printf("\n\n");
        for(int i=0;i<n;i++){
		if(arr[i]!=0) printf("-|");
		else printf("-");
	}
	printf("\n");
	for(int i=0;i<n;i++){
		if(arr[i]!=0) printf(" %d",arr[i]);
		else printf(" ");
	}
	printf("\n\nAverage waiting time : %.2f\nAverage Turnaround time : %.2f\n",(float)totw/p,(float)tott/p);
	return;
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

void roundRobin(struct queue * head,int n,int ts,int tt){
	int clk[tt],flag=0;
	for(int i=0;i<tt;i++) clk[i]=0;
	struct queue * readyQ = NULL;
	struct queue * doneQ = NULL;

	while(head!=NULL && head->arTime<=clock ){
		readyQ = enQ(readyQ,head);
		head = deQ(head);
	}
	
	while(readyQ!=NULL){
		flag=0;
		struct queue * temp = readyQ;
		
		int waTime = clock - (temp->arTime);
		int index = temp->ind;
		temp->time[index] = clock;
		index++;
		temp->ind = index;
		(temp->p->wt) += waTime;
		(temp->p->tat) += waTime;
		for(int i=0;i<ts;i++){
			int bt = temp->p->bTime;
			if(bt==0) {
				readyQ->time[index] = clock;
				index++;
				readyQ->ind = index;
				flag=1;
				doneQ = enQ(doneQ,readyQ);
				readyQ=deQ(readyQ);
				break;
			}else{
				temp->p->bTime = bt-1;
				(temp->p->tat)+=1;
				clock++;
			}
			//arrival of processes from jobQ to readyQ according to arrival time 
			while(head!=NULL && head->arTime<=clock ){
				readyQ = enQ(readyQ,head);
				head = deQ(head);
			}
		}
		if(flag!=1 && (temp->p->bTime) == 0){
			readyQ->time[index] = clock;
			index++;
			readyQ->ind = index;
			flag=1;
			doneQ = enQ(doneQ,readyQ);
			readyQ=deQ(readyQ);
		}
		if(flag!=1){
			readyQ->time[index] = clock;
			index++;
			readyQ->ind = index;
			temp->arTime=clock;
			readyQ = enQ(readyQ,readyQ);
			readyQ = deQ(readyQ);
		}
		
		clk[clock-1]=clock;
	}
	printGant(doneQ,clk,tt,n);
	return ;
}

int main(){
	FILE *ip;
	ip = fopen("input.txt","r");
	int n,ts,p,a,b;
	struct queue * jobQ = NULL;
	fscanf(ip,"%d%d",&n,&ts);
	int tt=0;
	for(int i=0;i<n;i++){
		fscanf(ip,"%d%d%d",&p,&a,&b);
		tt+=b;
		jobQ=enqueue(jobQ,a,b,p);
	}
	fclose(ip);
	sort(&jobQ,n);
	roundRobin(jobQ,n,ts,tt);	
	free(jobQ);
	
}

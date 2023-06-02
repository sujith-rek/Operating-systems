/*
Preemptive priority scheduling algorithm
*/


#include <stdio.h>
#include <stdlib.h>

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

int clock =0;

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
	int totalWTime=0,totaltat=0;
	printf("PID\tTurnaround time \tWaiting time\n");
	while(temp!=NULL){
		int Pid = temp->p->pid;
		int WAT = temp->waTime;
		int TAT = temp->p->tat + WAT;
		totaltat+=TAT;
		totalWTime+=WAT;
		printf("%d\t\t%d\t\t%d\n",Pid,TAT,WAT);
		temp=temp->next;
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

struct queue * swap (struct queue * q1, struct queue * q2){
	struct queue * temp = q2->next;
	q2->next = q1;
	q1->next = temp;
	return q2;
}

void schedule(struct queue ** head,int n){
	if(n==1) return ;
	for(int i=0;i<n;i++){
		struct queue ** temp = head;
		for(int j=0;j<n-1;j++){
			struct queue * q1 = *temp;
			struct queue * q2 = q1->next;
			if((q1->p->bTime)<(q2->p->bTime) && (q1->p->bTime)!=(q2->p->bTime)){
				*temp = swap(q1,q2);
			}else{
                if((q1->p->bTime)==(q2->p->bTime)){
                    if((q1->arTime)>(q2->arTime)){
                        *temp = swap(q1,q2);
                    }else{
                        if((q1->p->pid)>(q2->p->pid))
                            *temp = swap(q1,q2);
                    }
                }
            }
			temp = &(*temp)->next;
		}	
	}
	return ;
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

void execute(struct queue * head,int n){
	struct queue * readyQ = NULL;
	struct queue * doneQ = NULL;
	int size = 0;
	while(head!=NULL && head->arTime<=clock ){
		readyQ = enQ(readyQ,head);
		head = deQ(head);
		size++;
	}
	schedule(&readyQ,size);
	
	while(readyQ!=NULL || head!=NULL){
		if(readyQ!=NULL){
            struct queue * temp = readyQ;
            (temp->waTime) += (clock - (temp->arTime)) ;
            (temp->p->tat)++;
            (temp->p->bTime)--;
            clock++;
            temp->arTime = clock;
            printf("%d pid\n",temp->p->pid);
            if(temp->p->bTime==0){
                readyQ = deQ(readyQ);
                doneQ = enQ(doneQ,temp);
                size--;
            }
            while(head!=NULL && head->arTime<=clock ){
                readyQ = enQ(readyQ,head);
                head = deQ(head);
                size++;
            }
		}
		else{
			clock++;
			while(head!=NULL && head->arTime<=clock ){
				readyQ = enQ(readyQ,head);
				head = deQ(head);
				size++;
			}
		}
		if(readyQ!=NULL) schedule(&readyQ,size);
	}
	printGant(doneQ,n);
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



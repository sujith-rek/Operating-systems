#include<stdio.h>
#include<stdlib.h>

int n = 5, m = 3;
int alloc[5][3] = { 
    { 0, 1, 0 }, // P0 // Allocation Matrix
    { 2, 0, 0 }, // P1
    { 3, 0, 2 }, // P2
    { 2, 1, 1 }, // P3
    { 0, 0, 2 }  // P4
}; 

int max[5][3] = { 
    { 7, 5, 3 }, // P0 // MAX Matrix
    { 3, 2, 2 }, // P1
    { 9, 0, 2 }, // P2
    { 2, 2, 2 }, // P3
    { 4, 3, 3 } // P4
}; 

int avail[3] = { 3, 3, 2 }; // Available Resources

//Need matrix = max - alloc
int need[5][3] = { 
    { 7, 4, 3 }, // P0
    { 1, 2, 2 }, // P1
    { 6, 0, 0 }, // P2
    { 0, 1, 1 }, // P3
    { 4, 3, 1 } // P4
};

void calculateNeed(){
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            need[i][j] = max[i][j] - alloc[i][j];
        }
    }
}

int safeSequence[5];

int main(){
    int finish[n],ind=0;
    for(int i = 0; i < n; i++) finish[i] = 0;
    calculateNeed();
    for(int loop=0;loop<n;loop++){
        for(int i=0;i<n;i++){
            int flag = 0;
            for(int j=0;j<m;j++){
                if(need[i][j]<=avail[j] && finish[i]==0){
                    flag++;
                }
            }
            if(flag==m){
                for(int j=0;j<m;j++){
                    avail[j] += alloc[i][j];
                }
                finish[i] = 1;
                safeSequence[ind++] = i;
            }
        }
        int check=0;
        for(int done=0;done<n;done++){
            if(finish[done]==1) check++;
        }
        if(check==n)    break;
    }

    int flag=0;
    for(int i=0;i<n;i++)    if(finish[i]==1) flag++;
    
    if(flag==n){
        printf("the safe sequence is: ");
        for(int i=0;i<n;i++)    printf("P%d ",safeSequence[i]);
    }
    else{
        printf("the system is not in safe state");
    }

}
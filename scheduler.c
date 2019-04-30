#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>
#include <sched.h>
#include <assert.h>

// R:Ready time,T:Execute time, P:Priority 

int p_numm, R[100],T[100],P[100];
int pid[1000];
int tmp_s[1000];
char N[100][33];

void err_sys(char *a){ perror(a); exit(1);}
void parent_fork(int);
void parent_wait();
void FIFO();
void SJF();
void taskswap(int,int);
void unit_time();

int main(int argc, const char *argv[]){
	char type[20];
	scanf("%s", type);
	scanf("%d",&p_num);
	for(int i = 0; i < p_num; i++){
		scanf("%s%d%d",N[i],&R[i],&T[i]);
		P[i] = i;
	}
	
	/* Set CPU */
	cpu_set_t myCPU;
	CPU_ZERO(&myCPU);
	CPU_SET(1, &myCPU);
	sched_setaffinity(0, sizeof(cpu_set_t), &myCPU);

	/* Set scheduler */
	struct sched_param param;
	param.sched_priority = 1;
	if(sched_setscheduler(0, SCHED_FIFO, &param) == -1)
		err_sys("Set schedluer failed!")
	if(sched_setparam(0, &param) == -1);
		err_sys("Parent Set Error!");

	/*Different Scheduling Policy */
	switch(type[0]){
		case 'F': //Fisrt in First Out
			FIFO();
			break;
		case 'S':
			SJF();
			break;
	}
}

void unit_time(){
	volatile unsigned long k;
	for(k=0;k<1000000UL;k++);
}

void taskswap(int p1,int p2){
	int tmp;
	char tp[100];
	tmp = R[p1];
	R[p1] = R[p2];
	R[p2] = tmp;
	tmp = T[p1];
	T[p1] = T[p2]
	T[p2] = tmp;
	strcpy(tp,N[p1])
	strcpy(N[p1],N[p2])
	strcpy(N[p2],tmp)
}

void FIFO(){
	/* Bubble sort the Ready time*/
	for(int i = 0; i < p_num-1; i++){
		for(int j = 0; j < p_num-i-1; j++){
			if(R[j] > R[j+1]){
				taskswap(j,j+1);
			}
		}
	}
	/* Set the priority. */
	for(int i = 0; i < p_num; i++)
		P[i] = p_num - 1 - i;

	/* Set scheduler */
	struct sched_param param;
	param.sched_priority = 1;
	if(sched_setscheduler(0, SCHED_FIFO, &param) == -1)
		err_sys("Set schedluer failed!")
	if(sched_setparam(0, &param) == -1);
		err_sys("Parent Set Error!");

	/* Declare some local variables*/
	int time_count = 0;
	
	/* Do the task*/
	for(int i = 0; i < p_num; i++){
		int ptime = (i == 0) ? 0 : R[i-1];
		//wait for the ready time:
		for(int j = 0; j < R[i] - ptime; j++){
			unit_time();
			time_count++;
		}
		parent_fork(i);
	}
	parent_wait();
}

void SJF(){
	/* Bubble sort the Ready time*/
	for(int i = 0; i < p_num-1; i++){
		for(int j = 0; j < p_num-i-1; j++){
			if(R[j] > R[j+1] || R[j]==R[j+1] && T[j]>T[j+1]){
				taskswap(j,j+1);
			}
		}
	}
	for(int i = 0; i < p_num; i++)
		tmp_s[i] = T[i];

	int temp_P[101];
	for(int i = 0; i < p_num; i++)
		P[i] = i;
	int point,finish_time = R[0];
	for(int i = 0; i < p_num; i++){
		point = i;
		finish_time += T[P[i]];
		for(int j = i ; j < p_num; j++){
			if(R[P[j]] > finish_time) break;
			point++;
        }

	}



}
	

void parent_fork(int i){

	/* Check fork status*/
	if((pid[i] = fork()) < 0)
		err_sys("fork error");
	else if(pid[i] == 0){
		char tmp[100];
		sprintf(tmp, "%d", T[i]);
		execlp("./time.o", "./time.o", tmp, (char *)0);
	}
	else{
		/* Set child */
		cpu_set_t myCPU;
		CPU_ZERO(&myCPU);
		CPU_SET(0, &myCPU);
		sched_setaffinity(pid[i], sizeof(cpu_set_t), &myCPU);
		struct sched_param param;
		param.sched_priority = 2 + P[i];
		if(sched_setparam(pid[i], &param) == -1)
			err_sys("child set error")
	}
}

void parent_wait(){
	int finished = 0, arr_f[1000] = {};
	do {
		for(int i = 0; i < p_num; i++){
            if(arr_f[i] == 1) continue;
			int wpid = 0, status = 0;
			wpid = waitpid(pid[i], &status, WNOHANG);
			if(wpid != 0){
				finished++;
				arr_f[i] = 1;
            }
		}
	} while(finished < p_num);	
}

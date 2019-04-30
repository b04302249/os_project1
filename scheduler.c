/******************************************************************************
  filename          [ scheduler.c ]
  packagename       [ os_project_1]
  synopsis          [ implement for scheduler ]
******************************************************************************/  

#include "scheduler.h"

// R:Ready time,T:Execute time, P:Priority 

void swap(int *x, int *y){ int tem = *x; *x = *y; *y = tem;}

void err_sys(char *a){ perror(a); exit(1);}


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
	T[p1] = T[p2];
	T[p2] = tmp;
	strcpy(tp,N[p1]);
	strcpy(N[p1],N[p2]);
	strcpy(N[p2],tmp);
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
		err_sys("Set schedluer failed!");
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
			err_sys("child set error");
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

void RR(){
    for(int i = 0; i < p_num-1; i++)
        for(int j = 0; j < p_num-i-1; j++)
            if(R[j] > R[j+1]){ swap(&R[j], &R[j+1]); swap(&T[j], &T[j+1]); \
                char tmp[33]; strcpy(tmp, N[j]); strcpy(N[j], N[j+1]); strcpy(N[j+1], tmp);}

    //printf("After sorting:\n\n");
    for(int i = 0; i < p_num; i++){
        P[i] = p_num - 1 - i;
        //printf("%s %d %d\n", N[i], R[i], T[i]);
    }
    //printf("\n\n");

    //for(int i = 0; i < p_num; i++) P[i] = i;
    /* Set scheduler */
    struct sched_param param;
    param.sched_priority = 1;
    if(sched_setscheduler(0, SCHED_FIFO, &param) == -1){
        err_sys("set schedluer failed");
    }
    if(sched_setparam(0, &param) == -1){
        err_sys("parent set error");
    }
    int time_count = 0, start_time = R[0], head = 0, tail = 0;
    int queue[1001],arr_f[1001] = {};
    for(int i = 0; i < p_num; i++){

        int ptime = (i == 0) ? 0 : R[i-1];
        for(int j = 0; j < R[i] - ptime; j++){
            
            //if(arr_f[queue[kk]] == 1) continue;
            int wpid = 0, status = 0;
            if(head != tail){
            	wpid = waitpid(pid[queue[head]], &status, WNOHANG);
            	if(wpid != 0){
            	    start_time = time_count;
            	    arr_f[queue[head++]] = 1;
                    // printf("end waiting %s\n", N[queue[head - 1]]);
            	}	
            }
        
            if(head == tail) start_time = time_count;
            if(time_count-start_time == 500){
                int temp = queue[head];
                start_time = time_count;
                //printf("After Round-Robin:\nThe sequence of process is [");
                for(int k = head; k < tail-1; k++){
                    queue[k] = queue[k+1];
                    //printf("%d ",queue[k]);
                }
                queue[tail - 1] = temp;
                //printf("%d]\n",queue[tail - 1]);
                for(int k = head; k < tail; k++){
                    P[queue[k]] = p_num - 1 - k; 
                }
            	param.sched_priority = 2 + P[queue[tail - 1]] - 1;
            	if(sched_setparam(pid[queue[tail - 1]], &param) == -1){
            	    err_sys("child set error");
            	}
                for(int k = head; k < tail; k++){
                    param.sched_priority = 2 + P[queue[k]];
                    if(sched_setparam(pid[queue[k]], &param) == -1){
                        err_sys("child set error");
                    }

                }
            }
            volatile unsigned long k;
            for(k=0;k<1000000UL;k++);
            time_count++;
        }
    	//fprintf(stderr, "parent fork: %s\n", N[i]);
    	if((pid[i] = fork()) < 0){
    	    err_sys("fork error");
    	}
    	else if(pid[i] == 0){ /* child */
            /* Reset scheduler */
            //struct sched_param param;
            //print_affinity();
            char tmp[100];
            sprintf(tmp, "%d", T[i]);
            //fprintf(stderr, "I start at time %d, p=%d.\n",time_count, 2 + P[i]);
            execlp("./time.o", "./time.o", tmp, (char *)0);
    	}
    	else{

            queue[tail++] = i;
            /* Reset CPU */ 
            cpu_set_t my_set;
            CPU_ZERO(&my_set);
            CPU_SET(0, &my_set);
            sched_setaffinity(pid[i], sizeof(cpu_set_t), &my_set);
            /* set child priority */
            param.sched_priority = 2 + P[i];
            if(sched_setparam(pid[i], &param) == -1){
                err_sys("child set error");
            }
    	}	
		
    }
    //parent_wait();
    do {
        int wpid = 0, status = 0;
        wpid = waitpid(pid[queue[head]], &status, WNOHANG);
        if(head != tail){
            if(wpid != 0)
                {
                    if(wpid == -1) err_sys("wait child error!");
                    start_time = time_count;
        	    arr_f[queue[head++]] = 1;
        	    //printf("end waiting %s\n", N[queue[head - 1]]);
        	}
        }
        if(head == tail) start_time = time_count;
        if(time_count-start_time == 500){
            int temp = queue[head];
            start_time = time_count;
            //printf("After Round-Robin:\nThe sequence of process is [");
            for(int k = head; k < tail-1; k++){
                queue[k] = queue[k+1];
                // printf("%d ",queue[k]);
            }
            queue[tail - 1] = temp;
            //printf("%d]\n",queue[tail - 1]);
            for(int k = head; k < tail; k++){
                P[queue[k]] = p_num - 1 - k; 
            }
            param.sched_priority = 2 + P[queue[tail - 1]] - 1;
            if(sched_setparam(pid[queue[tail - 1]], &param) == -1){
                err_sys("child set error");
            }
            for(int k = head; k < tail; k++){
                param.sched_priority = 2 + P[queue[k]];
                if(sched_setparam(pid[queue[k]], &param) == -1){
                    err_sys("child set error");
                }
            }
			
        }
        volatile unsigned long k;
        for(k=0;k<1000000UL;k++);
        time_count++;
    } while(head != p_num || head != tail);
}





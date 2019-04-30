/******************************************************************************
  filename          [ main.c ]
  packagename       [ os_project_1]
  synopsis          [ main ]
******************************************************************************/  

#include "scheduler.h"




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
		err_sys("Set schedluer failed!");
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
        case 'R':
            RR();
            break;

	}
}


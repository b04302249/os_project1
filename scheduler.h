/******************************************************************************
  filename          [ scheduler.h ]
  packagename       [ os_project_1]
  synopsis          [ header file for scheduler ]
******************************************************************************/  

#ifndef SCHEDULER_H
#define SCHEDULER_H
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

int p_num;
int *R, *T, *P, *pid;
int tmp_s[1000];
char N[100][33];


void swap(int *xp, int *yp);
void err_sys(char* a);
void unit_time();
void taskswap(int, int);
void FIFO();
void SJF();
void parent_fork(int);
void parent_wait();
void RR();

#endif

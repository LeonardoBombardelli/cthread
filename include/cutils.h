/*
Implementação da biblioteca CThread.
Para mais informações, leia o README.md disponível no repositório.
Autores: Gabriel Tadiello, Laura Soares e Leonardo Bombardelli
*/

#ifndef __CUTILS_
#define __CUTILS_

#include "../include/cdata.h"
#include "../include/support.h"
#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>

#define STACK_SIZE 4096
#define DEFAULTPRIORITY 0
#define INITIALTID 1

//For the meanwhile, i'm using this code as a guideline for the use of the ucontext primitives
//https://gist.github.com/DanGe42/7148946


// Struct containing the TID of a thread that's waiting for another (and the TID from the other thread)
typedef struct cjs{
  int waitingTID;
  int beingWaitedTID;
} cjoin_struct;


//Queues for each state a thread can be in:
extern FILA2 readyQueue;
extern FILA2 blockedQueue;
extern FILA2 suspenseReadyQueue;
extern FILA2 suspenseBlockedQueue;
extern TCB_t *runningThread;

extern TCB_t mainThread;

extern ucontext_t endExecSchedulerContext; //Routine for scheduling that needs to be called after a thread ends. Every thread's uc_link needs to point at it's address.
extern char endExecSchedulerStack[STACK_SIZE];

extern ucontext_t yieldContext;
extern char yieldStack[STACK_SIZE];

extern int numberOfThreadsCreated; //Can be used to see if we already made a thread


int scheduler();

void endExecScheduler();

int InitializingCThreads();

// Returns the TID of the thread that was waiting for the thread with TID "beingWaitedTID" (if there is one)
// If finds a thread waiting for the "beingWaitedTID", it also removes the cjoin_struct from the queue
// If there's no thread waiting for it, *ok == 0 (*ok == 1 otherwise)
int getThreadWaitingFor(int beingWaitedTID, int *ok)

// Removes a thread from the suspendedQueue and insert it to the readyQueue
// Returns 0 upon success, -1 otherwise (probably won't be neccessary tho)
int removeFromBlocked(int tid)


#endif

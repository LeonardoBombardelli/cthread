/*
Implementação da biblioteca CThread.
Para mais informações, leia o README.md disponível no repositório.
Autores: Gabriel Tadiello, Laura Soares e Leonardo Bombardelli
*/

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/cutils.h"
#include "../include/support.h"
#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>


//For the meanwhile, i'm using this code as a guideline for the use of the ucontext primitives
//https://gist.github.com/DanGe42/7148946

//Queues for each state a thread can be in:
FILA2 readyQueue;
FILA2 blockedQueue;
FILA2 suspenseReadyQueue;
FILA2 suspenseBlockedQueue;
FILA2 cjoinQueue;
TCB_t *runningThread;

TCB_t mainThread;

ucontext_t endExecSchedulerContext; //Routine for scheduling that needs to be called after a thread ends. Every thread's uc_link needs to point at it's address.
char endExecSchedulerStack[STACK_SIZE];

ucontext_t yieldContext;
char yieldStack[STACK_SIZE];

int numberOfThreadsCreated = INITIALTID; //Can be used to see if we already made a thread



int scheduler()
{
    if(FirstFila2(&readyQueue) != 0)
    {
        return 0;
    }

    FirstFila2(&readyQueue);
    runningThread = (TCB_t *)GetAtIteratorFila2(&readyQueue);
    DeleteAtIteratorFila2(&readyQueue);
    runningThread->state = PROCST_EXEC;
    setcontext(&runningThread->context);
    return 0;
}

void endExecScheduler() 
{
    int ok = 0;
    int finishedTID = runningThread->tid;
    int waitingTID = getThreadWaitingFor(finishedTID, &ok);

    // If there was a thread waiting for the thread that has just finished it
    if(ok == 1){
      removeFromBlocked(waitingTID);  // Removes it from suspended
    }

    // runningThread has finished its execution
    free(runningThread);
    scheduler();
}

int InitializingCThreads()
{
    int initializationOk = 0;
    //Creates all queues
    initializationOk |= CreateFila2(&readyQueue);
    initializationOk |= CreateFila2(&blockedQueue);
    initializationOk |= CreateFila2(&suspenseReadyQueue);
    initializationOk |= CreateFila2(&suspenseBlockedQueue);
    initializationOk |= CreateFila2(&cjoinQueue);

    //Creates context of endExecSchedulerContext, to which every other thread will link to
    getcontext(&endExecSchedulerContext);
    endExecSchedulerContext.uc_link = 0;
    endExecSchedulerContext.uc_stack.ss_sp = endExecSchedulerStack;
    endExecSchedulerContext.uc_stack.ss_size = STACK_SIZE;
    makecontext(&endExecSchedulerContext, (void(*)(void)) endExecScheduler, 0);

    //Creates context of yieldContext, called everytime we make a cyield call
    getcontext(&yieldContext);
    yieldContext.uc_link = 0;
    yieldContext.uc_stack.ss_sp = yieldStack;
    yieldContext.uc_stack.ss_size = STACK_SIZE;
    makecontext(&yieldContext, (void(*)(void)) scheduler, 0);

    //Creates thread main, with TID 0
    mainThread.tid = 0;
    mainThread.prio = DEFAULTPRIORITY;
    mainThread.state = PROCST_EXEC;
    getcontext(&mainThread.context);

    runningThread = &mainThread;

    return initializationOk;
}

// Returns the TID of the thread that was waiting for the thread with TID "beingWaitedTID" (if there is one)
// If finds a thread waiting for the "beingWaitedTID", it also removes the cjoin_struct from the queue
// If there's no thread waiting for it, *ok == 0 (*ok == 1 otherwise)
int getThreadWaitingFor(int beingWaitedTID, int *ok)
{
  int found = 0;
  int waitingTID = -1;
  cjoin_struct *cj = NULL;  

  FirstFila2(&cjoinQueue);
  cj = (cjoin_struct*) GetAtIteratorFila2(&cjoinQueue);

  while(cj != NULL && found == 0){    
    if(cj->beingWaitedTID == beingWaitedTID){
      // Found a thread waiting for the "beingWaitedTID"
      DeleteAtIteratorFila2(&cjoinQueue);
      found = 1;
      waitingTID = cj->waitingTID;
    } else{
      // Otherwise, advances in the queue
      NextFila2(&cjoinQueue);
      cj = (cjoin_struct*) GetAtIteratorFila2(&cjoinQueue);
    }
  }

  // Thread is ready again, we can free the corresponding cjoin_struct
  free(cj);
  *ok = found;
  return waitingTID;
}

// Removes a thread from the suspendedQueue and insert it to the readyQueue
// Returns 0 upon success, -1 otherwise (probably won't be neccessary tho)
int removeFromBlocked(int tid)
{
  int found = 0;  
  TCB_t *aThread;

  FirstFila2(&blockedQueue);
  aThread = (TCB_t*) GetAtIteratorFila2(&blockedQueue);

  while(aThread != NULL && found == 0){
    if(aThread->tid == tid){
      // Found the thread that was blocked
      DeleteAtIteratorFila2(&blockedQueue);
      AppendFila2(&readyQueue, (void*) aThread);
      found = 1;
    } else{
      // Otherwise, advances in the queue
      NextFila2(&blockedQueue);
      aThread = (TCB_t*) GetAtIteratorFila2(&blockedQueue); 
    }
  }

  return found;
}

// Returns 1 if it has found tid at the queue, 0 otherwise
int searchFor(PFILA2 queue, int tid)
{
  int found = 0;
  TCB_t *aThread;

  FirstFila2(queue);
  aThread = (TCB_t*) GetAtIteratorFila2(queue);

  while(aThread != NULL && found == 0){
    if(aThread->tid == tid){
      // Found the thread that was blocked
      found = 1;
    } else{
      // Otherwise, advances in the queue
      NextFila2(queue);
      aThread = (TCB_t*) GetAtIteratorFila2(queue); 
    }
  }

  return found;
}

// Returns 1 if it has found tid as beingWaitedTID in the cjoinQueue, 0 otherwise
// Used to check if there is already a thread waiting for tid
int searchAtcjoinQueue(int tid)
{
  int found = 0;
  int waitingTID = -1;
  cjoin_struct *cj = NULL;  

  FirstFila2(&cjoinQueue);
  cj = (cjoin_struct*) GetAtIteratorFila2(&cjoinQueue);

  while(cj != NULL && found == 0){    
    if(cj->beingWaitedTID == tid){
      // Found a thread waiting for the "beingWaitedTID"
      found = 1;
    } else{
      // Otherwise, advances in the queue
      NextFila2(&cjoinQueue);
      cj = (cjoin_struct*) GetAtIteratorFila2(&cjoinQueue);
    }
  }

  return found;
}

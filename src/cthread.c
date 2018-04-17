/*
Implementação da biblioteca CThread.
Para mais informações, leia o README.md disponível no repositório.
Autores: Gabriel Tadiello, Laura Soares e Leonardo Bombardelli
*/

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"
#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>

#define STACK_SIZE 4096
#define DEFAULTPRIORITY 0
#define INITIALTID 1

//For the meanwhile, i'm using this code as a guideline for the use of the ucontext primitives
//https://gist.github.com/DanGe42/7148946

//Queues for each state a thread can be in:
FILA2* readyQueue;
FILA2* blockedQueue;
FILA2* suspenseReadyQueue;
FILA2* suspenseBlockedQueue;
TCB_t *runningThread;

int numberOfThreadsCreated = INITIALTID; //Can be used to see if we already made a thread

void InitializeQueues()
{
    CreateFila2(readyQueue);
    CreateFila2(blockedQueue);
    CreateFila2(suspenseReadyQueue);
    CreateFila2(suspenseBlockedQueue);            
}

void scheduler()
{
    //Debugging only, shows thread that is getting out of execution
    printf("Scheduling out thread %d\n", runningThread->tid);

    if(FirstFila2(readyQueue) != 0) //Sets iterator to the first element of readyQueue, !=0 means queue is empty
    {
        runningThread = (TCB_t *)GetAtIteratorFila2(readyQueue);
        printf("Scheduling in thread %d\n", runningThread->tid);

        setcontext(&runningThread->context);
    }
}

int ccreate (void* (*start)(void*), void *arg, int prio)
{  
    if(numberOfThreadsCreated == INITIALTID) {InitializeQueues();}

    TCB_t* newTCB;
    void* newStack;
    ucontext_t *newContext;

    //Initializing TCB
    newTCB->prio = 0;
    newTCB->state = PROCST_APTO;
    newTCB->tid = numberOfThreadsCreated;
    numberOfThreadsCreated++;
    newTCB->context = *newContext;

    AppendFila2(readyQueue, newTCB);

    //Creating new context
    getcontext(newContext);

    newStack = malloc(STACK_SIZE);
    if(newStack == NULL) return(-1);

    /*Initializing newContext stack*/
    newContext->uc_stack.ss_sp = newStack;
    newContext->uc_stack.ss_size = STACK_SIZE;
    newContext->uc_stack.ss_flags = 0;

    printf("New thread tid %d has been made", newTCB->tid);

    makecontext(newContext, start, arg);


    return(newTCB->tid); 
}

int main()
{
    return(0);
}
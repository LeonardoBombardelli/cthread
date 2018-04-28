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
FILA2 readyQueue;
FILA2 blockedQueue;
FILA2 suspenseReadyQueue;
FILA2 suspenseBlockedQueue;
TCB_t *runningThread;

TCB_t mainThread;

ucontext_t endExecSchedulerContext; //Routine for scheduling that needs to be called after a thead ends. Every thread's uc_link needs to point at it's address.
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

void endExecScheduler() //While we don't decide how we're implementing CJoin, it will be just a normal call to scheduler.
{
    scheduler();
}

void InitializingCThreads()
{
    //Creates all queues
    if(CreateFila2(&readyQueue) != 0) { printf("Erro ao criar fila readyQueue!"); }
    CreateFila2(&blockedQueue);
    CreateFila2(&suspenseReadyQueue);
    CreateFila2(&suspenseBlockedQueue);

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
}

int ccreate(void* (*start)(void*), void *arg, int prio)
{  
    TCB_t *newThread;

    if(numberOfThreadsCreated == INITIALTID) {InitializingCThreads();}

    //Initializes the new thread's TCB
    newThread = (TCB_t*)malloc(sizeof(TCB_t));
    newThread->prio = DEFAULTPRIORITY;
    newThread->state = PROCST_APTO;
    newThread->tid = numberOfThreadsCreated;
    
    //Initializes the new thread's context
    getcontext(&newThread->context);
    newThread->context.uc_link = &endExecSchedulerContext;
    newThread->context.uc_stack.ss_sp = (char *) malloc(STACK_SIZE);
    newThread->context.uc_stack.ss_size = STACK_SIZE;
    makecontext(&newThread->context, (void(*)(void))start, 1, arg);

    //Refresh number of threads created
    numberOfThreadsCreated++;

    if(AppendFila2(&readyQueue, (void *)newThread) != 0) { printf("Não pode inserir na fila"); }

    return newThread->tid;

}

int cyield( )
{
    TCB_t* yieldingThread;

    if(FirstFila2(&readyQueue) != 0){ return 0; }

    yieldingThread = runningThread;
    yieldingThread->state = PROCST_APTO;

    if(AppendFila2(&readyQueue, (void *)yieldingThread) != 0)
    {
        printf("Erro inserindo na fila");
        return -1;
    }

    swapcontext(&yieldingThread->context, &yieldContext);

    return 0;
}
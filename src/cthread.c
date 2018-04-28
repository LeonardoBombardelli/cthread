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



int ccreate(void* (*start)(void*), void *arg, int prio)
{  
    TCB_t *newThread;

    if(numberOfThreadsCreated == INITIALTID) 
    {
      int initializedCorrectly = InitializingCThreads();
      if(initializedCorrectly != 0){
          printf("Erro criando as filas\n");
          return -1;
      }
    }

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

    if(AppendFila2(&readyQueue, (void *)newThread) != 0)
    { 
        printf("Erro inserindo na fila\n");
        return -1;
    }

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
        printf("Erro inserindo na fila\n");
        return -1;
    }

    swapcontext(&yieldingThread->context, &yieldContext);

    return 0;
}

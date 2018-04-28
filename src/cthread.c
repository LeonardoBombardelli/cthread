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
      if(initializedCorrectly != 0)
      {
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

int cyield()
{
    TCB_t* yieldingThread;

    if(numberOfThreadsCreated == INITIALTID) 
    {
      int initializedCorrectly = InitializingCThreads();
      if(initializedCorrectly != 0)
      {
          printf("Erro criando as filas\n");
          return -1;
      }
    }

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

int cjoin(int tid)
{
    int found = 0;
    TCB_t *joiningThread;
    cjoin_struct *cj;

    if(numberOfThreadsCreated == INITIALTID) 
    {
      int initializedCorrectly = InitializingCThreads();
      if(initializedCorrectly != 0)
      {
          printf("Erro criando as filas\n");
          return -1;
      }
    }

    joiningThread = runningThread;

    if(searchAtcjoinQueue(tid) == 1)
    {
      // Some thread is already waiting for this tid
      return -1;
    }

    // Starts looking for the tid
    found = searchFor(&readyQueue, tid);
    if(found == 0)
    {
      // Needs to keep looking for tid
      found = searchFor(&blockedQueue, tid);
      if(found == 0)
      {
        // Needs to keep looking for tid
        found = searchFor(&suspenseReadyQueue, tid);
        if(found == 0)
        {
          // Needs to keep looking for tid
          found = searchFor(&suspenseBlockedQueue, tid);
          if(found == 0)
          {
            // Thread doesn't exists (not created yet or already has finished) or it's the same as the running thread
            return -1;
          }
        }
      }
    }
    
    // Correctly executed, running thread is now blocked
    joiningThread->state = PROCST_BLOQ;
    AppendFila2(&blockedQueue, (void*) joiningThread);

    // Establishes a link between the waiting thread and the one that it is waiting
    cj = (cjoin_struct*) malloc(sizeof(cjoin_struct));
    cj->waitingTID = runningThread->tid;
    cj->beingWaitedTID = tid;
    AppendFila2(&cjoinQueue, (void*) cj);

    // I'm not 100% sure that's right...
    swapcontext(&joiningThread->context, &yieldContext);
    return 0;
}

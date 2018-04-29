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

    if(FirstFila2(&readyQueue) != 0){ return -1; }

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

int csuspend (int tid)
{
    int returnCode;

    // verifica se o tid recebido ja se encontra nas filas de suspensos
    if((searchFor(&suspenseReadyQueue, tid) !=0 ) ||
       (searchFor(&suspenseBlockedQueue, tid) !=0))
    {
        printf("Erro: thread ja suspensa\n");
        return -1;
    } else {

        // verifica se o tid recebido pertence a thread que fez a chamada
        if(runningThread->tid==tid)
        {
            printf("Erro: thread suspendendo a si mesma\n");
            return -1;
        } else {

            // caso encontre o tid na fila de aptos
            if(searchFor(&readyQueue, tid) ==1)
            {
                returnCode = replaceThreadOnQueues(&readyQueue, &suspenseReadyQueue, PROCST_APTO_SUS, tid);
                if( returnCode != 0)
                {
                    printf("Erro suspendendo a thread\n");
                    return -1;
                } else return 0;
            } else {

                // caso encontre o tid na fila de bloqueados
                if(searchFor(&blockedQueue, tid) ==1)
                {
                    returnCode = replaceThreadOnQueues(&blockedQueue, &suspenseBlockedQueue, PROCST_BLOQ_SUS, tid);
                    if( returnCode != 0)
                    {
                        printf("Erro suspendendo a thread\n");
                        return -1;
                    } else return 0;
                } else {
                    printf("Erro: thread invalida ou inexistente\n");
                    return -1;
                }
            }
        }
    }
}

int cresume (int tid)
{
    int returnCode;

    // verifica se a thread recebida esta de fato suspensa
    if((searchFor(&readyQueue,tid) != 0) ||
       (searchFor(&blockedQueue,tid) !=0) ||
       (runningThread->tid==tid))
    {
        printf("Erro: thread nao esta suspensa\n");
        return -1;
    } else {

        // procura na fila de bloqueados suspensos
        if(searchFor(&suspenseBlockedQueue, tid) ==1)
        {
            returnCode = replaceThreadOnQueues(&suspenseBlockedQueue, &blockedQueue,
                                               PROCST_BLOQ, tid);
            if( returnCode != 0)
            {
                printf("Erro ao retomar a thread\n");
                return -1;
            } else return 0;
        } else {

            // procura na fila de aptos bloqueados
            if(searchFor(&suspenseReadyQueue, tid) ==1)
            {
                returnCode = replaceThreadOnQueues(&suspenseReadyQueue, &readyQueue,
                                                   PROCST_APTO, tid);
                if( returnCode != 0)
                {
                    printf("Erro ao retomar a thread\n");
                    return -1;
                } else return 0;
            } else {
                printf("Erro: thread invalida ou inexistente\n");
            }
        }
    }
}


int csem_init(csem_t *sem, int count)
{
    sem->count = count;
    sem->fila = malloc(sizeof(FILA2));
    if(CreateFila2(sem->fila) != 0) return -1;
    return 0;
}

int cwait(csem_t *sem)
{
    if(sem->count > 0)
    {
        sem->count--;
        return 0;
    }

    //If sem->count <= 0, thread goes blocked and we insert it's TID in the sem->fila.
    runningThread->state = PROCST_BLOQ;
    if(AppendFila2(&blockedQueue, (void *)runningThread) != 0) return -1;
    if(AppendFila2(sem->fila, (void *)runningThread->tid) != 0) return -1; //We insert the thread's TID in the semaphore's queue

    sem->count--;

    swapcontext(&runningThread->context, &yieldContext);
    return 0;
}

int csignal(csem_t *sem)
{
    int unlockedTID;
    if(FirstFila2(sem->fila) != 0) return -1;

    unlockedTID = (int)GetAtIteratorFila2(sem->fila);
    if(DeleteAtIteratorFila2(sem->fila) != 0) return -1;
    
    //The thread to be unlocked can be either in blocked or suspenseblocked
    if(searchFor(&blockedQueue, unlockedTID))
    {
        removeFromBlocked(unlockedTID);
        sem->count++;
        return 0;
    }

    if(searchFor(&suspenseBlockedQueue, unlockedTID))
    {
        removeFromSuspenseBlocked(unlockedTID);
        sem->count++;
        return 0;
    }

    return -1;
}
/* Testes para csuspend E cresume:
   condições de teste:

csuspend :
	(ok)(erro) thread inexistente
	(erro) thread ja suspensa (bloqueada)
                              (ok)(apta)
	(ok)(erro) thread suspendendo a si mesma
	thread bloqueada suspensa
	(ok)thread apta suspensa

cresume :
	(ok)(erro) thread inexistente
	(erro) thread não esta suspensa (bloqueada)
									(ok)(apta)
									(ok)(memsa thread que fez a chamada)
	thread voltando para bloqueada
	(ok)thread voltado para apta

 */
#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"
#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>


void *Quick(void *arg){
	int argument = (int) arg;
	printf("brown fox ");
	csuspend(argument);
    cyield();
    csuspend(argument); //erro: Fox ja esta apta-suspensa
    cresume(argument);
    cresume(argument); //erro: Fox ja voltou para apta

	return NULL;
}

void *Fox(void *arg){
    int argument = (int) arg;
    cyield();
    csuspend(argument); //erro: tid nao eh mais valido

    return NULL;
}

void *Lazy(void *arg){
    int argument = (int) arg;
    printf("The quick ");
    cresume(argument);
    return NULL;
}

void *Dog(void *arg){
    char *argument = (char*) arg;
    printf("%s\n", argument);
    return NULL;
}

int main(){
    char arg1[] = "a lazy dog.\n";

    int tid4 = ccreate(Dog, (void *) arg1, 0);
	int tid3 = ccreate(Lazy, (void *) tid4, 0);
    int tid2 = ccreate(Fox, (void *) tid3, 0);
    int tid1 = ccreate(Quick, (void *) tid2, 0);

    csuspend(tid4);
    cyield();
    printf("jumps over ");
    cyield();
    csuspend(0); //erro: main tentando suspender a si mesma
    cresume(0); //erro: thread nao esta suspensa
    cyield();
    cresume(tid1); //erro: tid1 nao eh mais valido

	printf("Owata!\n");
    return(1);
}

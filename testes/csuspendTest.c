/*	Testes para função csuspend!
 *		falta testar:
 *		suspender thread bloqueado;
 *		tentar suspender thread suspensa-bloqueada (erro);
 *		etc
 */

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"
#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>


void *MostraTexto(void *arg){
    char *argument = (char*) arg;
    printf("Hello there! \n");
	//tentando suspender uma thread que não existe:
	csuspend(5);
    cyield();
    printf("%s\n", argument);
    
    return NULL;
}

void *MostraNumeros(void *arg){
    char *argument = (char*) arg;
    printf("Ahoy here! \n");
    printf("%s\n", argument);
    
    return NULL;
}

void *WaitForIt(void *arg){
	int *argument = (int*) arg;
	printf("The quick brown fox...\n");
	csuspend(argument);
	return NULL;
}

int main(){

    char arg1[] = " jumps over a lazy dog";
    char arg2[] = "0 9 8 7 6 5 4 3 2 1";
    
    int tid1 = ccreate(MostraTexto, (void *) arg1, 0);
	int tid2 = ccreate(WaitForIt, (void *) tid1, 0);
    int tid3 = ccreate(MostraNumeros, (void *) arg2, 0);
	
	csuspend(tid3);
	//tentando suspender um thread já suspenso
	csuspend(tid3);
	
    printf("Main done creating its children, and suspending one of them\n");
    cyield();
    //mostraNumeros não deve ser executada, pois esta suspensa

    //main tentando se suspender:
    csuspend(0);
	
    printf("Fon\n");
	
    return(1);
}

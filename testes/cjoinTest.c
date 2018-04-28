/*
A simple test that probably won't work :)
*/

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"
#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>


void *MostraLixoNaTela(void *arg){
    char *argument = (char*) arg;
    cyield();
    printf("Hello there! \n");
    cyield();    
    printf("%s\n", argument);
    cyield();
    cyield();
    cyield();
    return NULL;
}

int main(){
    char arg1[] = "The quick brown fox jumps over a lazy dog";
    char arg2[] = "The lazy fox jumps over a quick brown dog";
    
    int tid1 = ccreate(MostraLixoNaTela, (void *) arg1, 0);
    int tid2 = ccreate(MostraLixoNaTela, (void *) arg2, 0);

    printf("Main done creating its children\n");
    cjoin(tid2);
    cjoin(tid1);

    cyield();
    cyield();
    printf("Fon\n");
    
    return(1);
}

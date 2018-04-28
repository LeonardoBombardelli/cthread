/*
A simple test that probably won't work :)
*/

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"
#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>


void *MostraLixoNaTela(void *gabriel){
    char *argument = (char*) gabriel;
    printf("%s\n", argument);
    return NULL;
}

int main(){
    char gtmoraes[] = "Gabriel Tadiello";
    char lbbombardelli[] = "Leonardo Bombardelli";
    ccreate(MostraLixoNaTela, (void *) gtmoraes, 0);
    ccreate(MostraLixoNaTela, (void *) lbbombardelli, 0);
    printf("Main done creating its children (sorry world): \n");
    cyield();
    return(1);
}

/*
A simple test that probably won't work :)
*/

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"
#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>

void MostraLixoNaTela()
{
    printf("Leonardo Bombardelli");
    cyield();
    printf("Fon");
}

int main()
{
    ccreate(MostraLixoNaTela, 0, 0);
    ccreate(MostraLixoNaTela, 0, 0);
    cyield();
    cyield();
    return(1);
}
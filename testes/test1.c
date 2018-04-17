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
}

int main()
{
    ccreate(MostraLixoNaTela, 0, NULL);
    return(1);
}
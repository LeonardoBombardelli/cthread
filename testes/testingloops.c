/*
A test where one thread throws the cyield to the other.
*/

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"
#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>

void LoopThread()
{
    int i;
    for(i = 0; i < 10; i++)
    {
        printf("%d\n", i);
        cyield();
    }
}

int main()
{
    int i;

    ccreate(LoopThread, 0, 0);
    cyield();
    
    for(i = 0; i < 10; i++)
    {
        printf("%d\n", i);
        cyield();
    }
    return(1);
}
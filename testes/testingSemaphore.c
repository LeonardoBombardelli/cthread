/*
A simple test that probably won't work :)
*/

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"
#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>

csem_t semaphore;

void* PrintsSomething()
{
    cwait(&semaphore);
    printf("and this should appear after that!\n");
    csignal(&semaphore);
    return NULL;
}

void* PrintsAnotherThing()
{
    cwait(&semaphore);
    printf("This will be the last one!");
    csignal(&semaphore);
    return NULL;
}

int main()
{
    csem_init(&semaphore, 1);
    cwait(&semaphore);
    ccreate(PrintsSomething, 0, 0);
    ccreate(PrintsAnotherThing, 0, 0);
    cyield();
    printf("This should appear first\n");
    csignal(&semaphore);
    cyield();
    cyield();
    return 0;
}
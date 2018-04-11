/*
Implementação da biblioteca CThread.
Para mais informações, leia o README.md disponível no repositório.
Autores: Gabriel Tadiello, Laura Soares e Leonardo Bombardelli
*/

#include "cdata.h"
#include "cthread.h"
#include "support.h"
#include <stdio.h>
#include <ucontext.h>


int ccreate (void* (*start)(void*), void *arg, int prio)
/*
start:	ponteiro para a função que a thread executará.
arg:	um parâmetro que pode ser passado para a thread na sua criação.
prio:	Não utilizado neste semestre, deve ser sempre zero.

Retorna identificador da thread se criada, ou um valor negativo em caso de erro.
*/
{
    return(-1); //Error value
}

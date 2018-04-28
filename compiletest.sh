gcc -m32 -c src/cthread.c -o bin/cthread.o
ar crs lib/libinclude.a bin/cthread.o bin/support.o
gcc -m32 -o testes/test1 testes/test1.c -L./lib -linclude -Wall
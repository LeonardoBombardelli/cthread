echo Insert file name:
read NAMEFILE
gcc -m32 -c src/cutils.c -o bin/cutils.o
gcc -m32 -c src/cthread.c -o bin/cthread.o
ar crs lib/libinclude.a bin/cutils.o bin/cthread.o bin/support.o
gcc -g -m32 -o testes/$NAMEFILE testes/$NAMEFILE.c -L./lib -linclude -Wall
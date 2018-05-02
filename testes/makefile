

all: test1 twoThreadsWithArguments cjoinTest csuspendTest joiningAlreadyJoinedThread testingloops testingSemaphore

test1: test1.c
	gcc test1.c -o test1.out -L../lib -lcthread

twoThreadsWithArguments: twoThreadsWithArguments.c
	gcc twoThreadsWithArguments.c -o twoThreadsWithArguments.out -L../lib -lcthread

cjoinTest: cjoinTest.c
	gcc cjoinTest.c -o cjoinTest.out -L../lib -lcthread

csuspendTest: csuspendTest.c
	gcc csuspendTest.c -o csuspendTest.out -L../lib -lcthread

joiningAlreadyJoinedThread: joiningAlreadyJoinedThread.c
	gcc joiningAlreadyJoinedThread.c -o joiningAlreadyJoinedThread.out -L../lib -lcthread

testingloops: testingloops.c
	gcc testingloops.c -o testingloops.out -L../lib -lcthread

testingSemaphore: testingSemaphore.c
	gcc testingSemaphore.c -o testingSemaphore.out -L../lib -lcthread


clean:
	rm *.out

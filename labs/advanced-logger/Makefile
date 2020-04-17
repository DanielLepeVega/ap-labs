
include ../../common.mk

logger.o:
	gcc -c logger.c

test: logger.o
	gcc -o test testLogger.c logger.o

runTest: test
	./test
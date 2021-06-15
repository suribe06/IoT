#
# Makefile for make
#
# Compiler: gcc
#

PROGS   = monitoreoCultivosFSM
CFLAGST = -D_REENTRANT -Wall
LIBS    = pthread
CC			= gcc

all: $(PROGS)

monitoreoCultivosFSM : phtrdsMsg.o monitoreoCultivos.o
	$(CC) $(CFLAGST) -o monitoreoCultivosFSM monitoreoCultivos.o phtrdsMsg.o -l $(LIBS)

phtrdsMsg.o : phtrdsMsg.c phtrdsMsg.h cultivosConf.h
	$(CC) $(CFLAGST) -g -c phtrdsMsg.c

monitoreoCultivos.o : monitoreoCultivos.c cultivosConf.h
	$(CC) $(CFLAGST) -g -c monitoreoCultivos.c

clean:
	rm -f $(PROGS) *~ *.o

#
# Makefile for make
#
# Compiler: gcc
#

PROGS   = monitoreoInvernaderosFSM
CFLAGST = -D_REENTRANT -Wall
LIBS    = pthread
CC			= gcc

all: $(PROGS)

monitoreoInvernaderosFSM : phtrdsMsg.o monitoreoInvernaderos.o
	$(CC) $(CFLAGST) -o monitoreoInvernaderosFSM monitoreoInvernaderos.o phtrdsMsg.o -l $(LIBS)

phtrdsMsg.o : phtrdsMsg.c phtrdsMsg.h invernaderoConf.h
	$(CC) $(CFLAGST) -g -c phtrdsMsg.c

monitoreoInvernaderos.o : monitoreoInvernaderos.c invernaderoConf.h
	$(CC) $(CFLAGST) -g -c monitoreoInvernaderos.c

clean:
	rm -f $(PROGS) *~ *.o

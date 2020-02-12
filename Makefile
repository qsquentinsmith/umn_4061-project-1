CFLAGS = -Wall -g
CC	   = gcc $(CFLAGS)

commando : commando.o cmd.o cmdcol.o util.o
	$(CC) -o commando commando.o cmd.o cmdcol.o util.o

util.o : util.c
	$(CC) -c util.c

cmdcol.o : cmdcol.c
	$(CC) -c cmdcol.c

cmd.o : cmd.c
	$(CC) -c cmd.c

commando.o : commando.c commando.h
	$(CC) -c commando.c

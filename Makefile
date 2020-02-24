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

clean :
	rm *.o commando
############################################################
# 'make zip' to create p1-code.zip for submission
AN=p1
SHELL  = /bin/bash
CWD    = $(shell pwd | sed 's/.*\///g')
zip : clean clean-tests
	rm -f $(AN)-code.zip
	cd .. && zip "$(CWD)/$(AN)-code.zip" -r "$(CWD)"
	@echo Zip created in $(AN)-code.zip
	@if (( $$(stat -c '%s' $(AN)-code.zip) > 10*(2**20) )); then echo "WARNING: $(AN)-code.zip seems REALLY big, check there are no abnormally large test files"; du -h $(AN)-code.zip; fi
	@if (( $$(unzip -t $(AN)-code.zip | wc -l) > 256 )); then echo "WARNING: $(AN)-code.zip has 256 or more files in it which may cause submission problems"; fi

# -*- indent-tabs-mode:t; -*-

# dclient Makefile

all: dclient

dclient: dclient.o md5.o
	clang dclient.o md5.o -o dclient -lsocket -lcrypto

dclient.o: dclient.c dclient.h
	clang -g -c dclient.c -Wall
	
md5.o: md5.c md5.h
	clang -g -c md5.c -Wall
	
test: dclient
	./dclient

check: dclient
	valgrind ./dclient

clean:
	rm -f dclient*.o *.txt *.jpg *.gz *.exe *.raw  *.mp3 core
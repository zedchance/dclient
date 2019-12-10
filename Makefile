# -*- indent-tabs-mode:t; -*-

# dclient Makefile

all: dclient

dclient: dclient.o md5.o
	clang dclient.o md5.o -o dclient -l socket -l crypto

dclient.o: dclient.c
	clang -g -c dclient.c -l socket -Wall
	
md5.o: md5.c md5.h
	clang -g -c md5.c -Wall
	
test: dclient
	./dclient

check: dclient
	valgrind ./dclient

clean:
	rm -f *.o dclient *.txt *.jpg *.gz *.exe *.raw core
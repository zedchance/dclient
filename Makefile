# -*- indent-tabs-mode:t; -*-

# dclient Makefile

dclient: dclient.c
	clang -Wall -g dclient.c -o dclient -l socket
	
test: dclient
	./dclient

clean:
	rm -f dclient *.txt *.jpg *.gz *.exe *.raw core
# -*- indent-tabs-mode:t; -*-

# dclient Makefile

dclient: dclient.c
	clang dclient.c -o dclient -l socket
CC = gcc
CFLAGS = -Wall

compile: main.c
	$(CC) $(CFLAGS) $< -o system_info


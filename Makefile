CC      = gcc
CFLAGS  = -Wall -pedantic -g -std=c99
OBJS    =  main.o loader.o 
TARGET	= main

.PHONY: all clean reallyclean

.SUFFIXES: .c .o

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS)

reallyclean: clean
	rm -f $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $*.c
CC=gcc
CFLAGS=-Wall -Wextra -Werror
BUILD=../build/lab1.exe

all: build

build: main.o
	$(CC) $(CFLAGS) main.o -o $(BUILD)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	del *.o

rebuild:
	clean all
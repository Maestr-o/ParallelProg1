CC=gcc
CFLAGS=-Wall -Wextra -Werror
BUILD=../build/lab1.exe
LDFLAGS=-fopenmp

all: build

build: main.o
	@$(CC) $(CFLAGS) $(LDFLAGS) main.o -o $(BUILD)

main.o: main.c
	@$(CC) $(CFLAGS) $(LDFLAGS) -c main.c -o main.o

clean:
	@del *.o

rebuild:
	clean all
# BRINDUSESCU Miruna 325CB #

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = tema1

all: build

build: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(TARGET)

run: build
	./$(TARGET)
     
test: build
	./checker.sh

clean:
	rm -f $(TARGET) tema1.in tema1.out rezultat_valgrind.txt

.PHONY: all build test clean
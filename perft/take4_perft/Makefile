# c compiler
CC = gcc
CFLAGS = -std=c2x -O2
CFLAGS += -Wextra -Wall -Wpedantic

# dirs
SRC = $(wildcard ../../take4/src/*.c) $(wildcard ../../take4/src/**/*.c)
ENTRY = perft.c
OUT = build
NAME = perft

.PHONY: build run clean

build:
	mkdir -p $(OUT)
	$(CC) -o $(OUT)/$(NAME) $(ENTRY) $(SRC) $(CFLAGS)

run: build
	$(OUT)/$(NAME)

clean:
	rm -rf $(OUT)/*
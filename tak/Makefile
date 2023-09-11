# c compiler
CC = gcc
CFLAGS = -std=c2x -O
CFLAGS += -Wextra -Wall -Wpedantic

# dirs
SRC = $(wildcard src/*.c)
DEV_SRC = $(wildcard dev/*.c)
OUT = build
NAME = auri

.PHONY: build run clean test

build:
	mkdir -p $(OUT)
	$(CC) -o $(OUT)/$(NAME) $(DEV_SRC) $(SRC) $(CFLAGS)

run: build

	$(OUT)/$(NAME)

clean:
	rm -rf $(OUT)/*

test: test/test.c
	$(CC) -o build/test test/test.c $(SRC) $(CFLAGS)
	build/test
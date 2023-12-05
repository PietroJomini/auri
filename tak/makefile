# c compiler
CC = gcc
CFLAGS = -std=c2x -O2
CFLAGS += -Wextra -Wall -Wpedantic

# dirs
OUT = bin
SRC = $(wildcard ./src/*.c) auri.c
INC = $(wildcard ./src/*.h)
TARGET = $(OUT)/auri

.PHONY: build test clean

build: $(TARGET)

$(TARGET): $(SRC) $(INC)
	$(CC) -o $@ $(SRC) $(CFLAGS)

clean:
	rm $(BINS)
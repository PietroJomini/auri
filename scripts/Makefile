# c compiler
CC = gcc
CFLAGS = -std=c2x -O2
CFLAGS += -Wextra -Wall -Wpedantic

# dirs
SRC = $(wildcard ../tak/src/*.c) $(wildcard ../tak/src/**/*.c)
SRC := $(filter-out %_dev.c,$(SRC))
ENTRY = pt2tps.c
OUT = bin
NAME = pt2tps

.PHONY: random clean

random:
	$(CC) -o $(OUT)/random $(SRC) random.c $(CFLAGS)

clean:
	rm -rf $(OUT)/*
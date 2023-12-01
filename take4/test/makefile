# c compiler
CC = gcc
CFLAGS = -std=c2x -O2
CFLAGS += -Wextra -Wall -Wpedantic

# dirs
OUT = bin
SRC = $(wildcard ../src/*.c) $(wildcard ../base/*.c)
INC = $(wildcard ../src/*.h) $(wildcard ../base/*.h)
TARGETS = $(wildcard *.c)
BINS = $(addprefix $(OUT)/,$(basename $(TARGETS)))

.PHONY: all run clean

all: $(BINS)
	$(BINS) | parallel -j0

$(OUT)/%: %.c $(SRC) $(INC)
	$(CC) -o $@ $(SRC) $< $(CFLAGS)

clean:
	rm $(BINS)
# c compiler
CC = gcc
CFLAGS = -std=c2x
CFLAGS += -Wextra -Wall -Wpedantic

# dirs
OUT = ./bin
TARGETS = $(wildcard *.c)
BINS = $(addprefix $(OUT)/,$(basename $(TARGETS)))
LIB = $(wildcard ../lib/*.h)

.PHONY: all clean

all: $(BINS)

$(OUT)/%: %.c $(LIB)
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -rf $(OUT)/*
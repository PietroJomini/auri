# c compiler
CC = gcc
CFLAGS = -std=c2x -O2
CFLAGS += -Wextra -Wall -Wpedantic

# dirs
OUT = ./bin
SRC = $(wildcard ../take4/src/*.c)
INC = $(wildcard ../take4/src/*.h)
TARGETS = $(wildcard ./*.c)
BINS = $(addprefix $(OUT)/,$(basename $(TARGETS)))

.PHONY: all clean

all: $(BINS)

$(OUT)/%: %.c $(SRC) $(INC)
	$(CC) -o $@ $(SRC) $< $(CFLAGS)

clean:
	rm -rf $(OUT)/*
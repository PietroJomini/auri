# compiler stuff
CC = gcc
CFLAGS = -std=c2x -O3 # -03
CFLAGS += -Wall -Wextra -Wpedantic
LFLAGS = # linker flags
RFLAGS = # run flags

# dirs
NAME = auri
BIND = ./bin
SRCD = ./src
SRC = $(wildcard $(SRCD)/*.c) $(wildcard $(SRCD)/**/*.c)
INC = $(wildcard $(SRCD)/*.h) $(wildcard $(SRCD)/**/*.h)
OBJ = $(SRC:$(SRCD)/%.c=$(BIND)/%.o)

.PHONY: all buikld run clean cneal-brute
all: run

# test suite
# tests requires criterion https://github.com/Snaipe/Criterion
test: NAME = test
test: SRC := $(filter-out %_dev.c,$(SRC))
test: SRC += test/test.c
test: LFLAGS += -lcriterion
test: RFLAGS += -j0
test: build run

# builds .o files to reduce compilation time
# as this allow to recompile only certain files
build: $(OBJ) $(INC)
	$(CC) $(OBJ) $(LFLAGS) -o $(BIND)/$(NAME)

# generic rule to compile c files
# to the respective .o
$(BIND)/%.o: $(SRCD)/%.c $(SRCD)/%.h
	@mkdir -p -- $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: build
	@$(BIND)/$(NAME) $(RFLAGS)

clean:
	rm $(OBJ) -f

# clean every file in the bin directory,
# including executable files
clean-brute:
	rm $(BIND)/* -rf

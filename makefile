CC=gcc
CC_FLAGS = -Wall -Wpedantic
CC_FLAGS += -O3

BIN = ./bin
LIBTAK_INC = $(wildcard lib/*)
TEI_SRC = $(wildcard tei/*.c)

.PHONY: tei clean

# double pass to actually check if we need to compile
tei: $(BIN)/tei

# compile tei
$(BIN)/tei: $(LIBTAK_INC) $(wildcard tei/*.h) $(TEI_SRC)
	mkdir -p $(BIN)
	$(CC) $(TEI_SRC) $(CC_FLAGS) -o $(BIN)/tei

# clean bin dir
clean:
	rm $(BIN)/*
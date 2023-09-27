#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../tak/src/auri.h"

void handle_line(char *tps, int upto) {
    Position p = tps2p(tps);
    printf("\"%s\"", tps);
    for (int i = 1; i <= upto; i++) printf(", %ld", perft(p, i));
    printf("\n");
}

int main() {
    char *line = NULL, *perft_upto_s = NULL;
    size_t len = 0, perft_upto = 0;

    // load $PERFT_UPTO
    perft_upto_s = getenv("PERFT_UPTO");
    if (perft_upto_s == NULL) perft_upto = 1;
    else perft_upto = atoi(perft_upto_s);

    while (getline(&line, &len, stdin) != -1) {
        // remove \n from end
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

        // handle tps
        handle_line(line, perft_upto);
    }
}
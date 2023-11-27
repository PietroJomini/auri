#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "../../take4/src/notation.h"
#include "../../take4/src/tak.h"

void handle_line(char *tps, int upto, slides_lt *slt) {
    position p = tps2position(tps, TPS_STD);
    struct timeval before, after, epoch;
    printf("%s", tps);

    for (int i = 1; i <= upto; i++) {
        gettimeofday(&before, NULL);
        uint64_t depth = perft(p, i, slt);
        gettimeofday(&after, NULL);
        timersub(&after, &before, &epoch);
        printf(", %ld, %ld", depth, epoch.tv_sec * 1000000 + epoch.tv_usec);
    }

    printf("\n");
}

int main() {
    char *line = NULL, *perft_upto_s = NULL;
    size_t len = 0, perft_upto = 0;

    // load $PERFT_UPTO
    perft_upto_s = getenv("PERFT_UPTO");
    if (perft_upto_s == NULL) perft_upto = 1;
    else perft_upto = atoi(perft_upto_s);

    // slt
    slides_lt slt = slt_fill();

    while (getline(&line, &len, stdin) != -1) {
        // remove \n from end
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

        // handle tps
        handle_line(line, perft_upto, &slt);
    }
}
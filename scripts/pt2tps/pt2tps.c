#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../tak/src/auri.h"

void pt2tps(char *buffer, char *pt, int size) {
    p2tps(buffer, playtak2position(pt, size));
}

int main() {
    // TODO: using 1024 is ulgy. It should be enough, but it should be better to check
    char *line = NULL, notation[1024] = {0}, tps[1024] = {0};
    size_t len = 0;
    int id, size;

    while (getline(&line, &len, stdin) != -1) {
        sscanf(line, "%d,%d,\"%[A-Z1-9, ]\"", &id, &size, notation);
        pt2tps(tps, notation, size);
        printf("%s\n", tps);
    }

    return EXIT_SUCCESS;
}
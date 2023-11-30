#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "../take4/src/notation.h"
#include "../take4/src/tak.h"

#define DEFAULT_SIZE 6
#define DEFAULT_DEPTH 3

void perftd(position p, int depth, zobrist_data *zd) {
    u64 nodes = 0, nnodes = 0;
    slides_lt slt = slt_fill();

    // load moves
    move buffer[500 + PLACEMENTS_MAX_AMOUNT];
    int n = search_moves(buffer, &p, &slt);

    char ptn[MAX_PTN_MOVE];

    for (int i = 0; i < n; i++) {
        nnodes = perft(do_move(p, buffer[i], zd), depth - 1, &slt, zd);
        nodes += nnodes;

        move2ptn(ptn, buffer[i], p.size, PTN_STD);
        printf("%s: %ld\n", ptn, nnodes);
    }

    printf("%ld\n", nodes);
}

int main(int argc, char **argv) {
    int c, size = DEFAULT_SIZE, depth = DEFAULT_DEPTH;

    while ((c = getopt(argc, argv, "s:d:")) != -1) {
        switch (c) {
            case 's': size = atoi(optarg); break;
            case 'd': depth = atoi(optarg); break;
            case '?': exit(EXIT_FAILURE);
        }
    }

    if (size < 3 || size > 8) size = DEFAULT_SIZE;
    if (depth < 1) depth = 1;

    zobrist_data zd = zobrist_fill();
    position p =
        (optind < argc) ? tps2position(argv[optind], TPS_STD, &zd) : new_position(size);
    perftd(p, depth, &zd);

    return EXIT_SUCCESS;
}
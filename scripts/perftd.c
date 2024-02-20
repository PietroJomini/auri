#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#define TAKLIB_IMPLEMENTATION
#define TAKLIB_NOHASH
#include "../lib/ptn.h"
#include "../lib/tak.h"
#include "../lib/ui.h"

#define DEFAULT_SIZE 6
#define DEFAULT_DEPTH 3

void perftd(tak_position p, int depth, int quiet, tak_zobrist_data *zd) {
    tak_slt slt = tak_slt_fill();
    if (quiet) printf("%ld\n", tak_perft(p, depth, &slt, zd));
    else {
        tak_print(p);

        tak_u64 nodes = 0, nnodes = 0;
        tak_move buffer[TAK_MAX_MOVES];
        int n = tak_search(buffer, &p, &slt);
        char ptn[PTN_MAX_MOVE_LENGTH];

        for (int i = 0; i < n; i++) {
            nnodes = tak_perft(tak_do(p, buffer[i], zd), depth - 1, &slt, zd);
            nodes += nnodes;

            ptn_encode(ptn, buffer[i], p.size, PTN_STD);
            printf("\n%d. %s: %ld", i, ptn, nnodes);
        }

        printf("\n\nNodes searched: %ld\n", nodes);
    }
}

int main(int argc, char **argv) {
    tak_zobrist_data zd = tak_zd_fill();
    int c, depth = DEFAULT_DEPTH, size = DEFAULT_SIZE, is_set = 0, quiet = 0;
    tak_position p;

    while ((c = getopt(argc, argv, "s:d:t:q")) != -1) {
        switch (c) {
            case 's':
                size = atoi(optarg);
                if (size >= 3 || size <= 8) {
                    p = tak_newposition(atoi(optarg));
                    is_set = 1;
                }
                break;
            case 't':
                p = tps_parse(optarg, TPS_STD, &zd);
                is_set = 1;
                break;
            case 'd': depth = atoi(optarg); break;
            case 'q': quiet = 1; break;
            case '?': exit(EXIT_FAILURE);
        }
    }

    if (depth < 1) depth = 1;
    if (!is_set) p = tak_newposition(size);
    if (optind < argc) p = ptn_apply(p, argv + optind, argc - optind, PTN_STD, &zd);

    perftd(p, depth, quiet, &zd);
    return EXIT_SUCCESS;
}
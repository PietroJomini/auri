#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define TAKLIB_IMPLEMENTATION
#define TAKLIB_NOHASH
#include "../lib/tak.h"
#include "../lib/ptn.h"

#define DEPTH_MIN 5
#define DEPTH_MAX 100
#define DEFAULT_SIZE 6
#define DEFAULT_AMOUNT 1

int randint(int a, int b) { return a + rand() % (b - a + 1); }

tak_position random_position(int size, int depth, tak_slt *slt, tak_zobrist_data *zd) {
    tak_position p = tak_newposition(size), np;
    tak_move buffer[TAK_MAX_MOVES];
    int t;

    while (--depth > 0) {
        t = tak_search(buffer, &p, slt);
        np = tak_do(p, buffer[randint(0, t - 1)], zd);

        // check if the game is ended
        if (tak_check_ending(&np, 0).ended) return p;
        else p = np;
    }

    return p;
}

int main(int argc, char **argv) {
    int c, depth_min = DEPTH_MIN, depth_max = DEPTH_MAX, size = DEFAULT_SIZE,
           amount = DEFAULT_AMOUNT;

    while ((c = getopt(argc, argv, "m:M:s:n:")) != -1) {
        switch (c) {
            case 'm': depth_min = atoi(optarg); break;
            case 'M': depth_max = atoi(optarg); break;
            case 's': size = atoi(optarg); break;
            case 'n': amount = atoi(optarg); break;
            case '?': exit(EXIT_FAILURE);
        }
    }

    if (size < 3 || size > 8) size = DEFAULT_SIZE;
    if (depth_min > depth_max) depth_min = depth_max;
    if (amount < 1) amount = 1;

    srand(time(NULL));
    tak_slt slt = tak_slt_fill();
    tak_zobrist_data zd = tak_zd_fill();
    char tps[TPS_MAX_LENGTH];

    for (int i = 0; i < amount; i++) {
        tak_position p = random_position(size, randint(depth_min, depth_max), &slt, &zd);
        tps_encode(tps, p, TPS_STD);
        printf("%s\n", tps);
    }

    return EXIT_SUCCESS;
}
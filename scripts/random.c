#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../tak/src/moves.h"
#include "../tak/src/notation/tps.h"
#include "../tak/src/position.h"

#define DEPTH_MIN 5
#define DEPTH_MAX 100
#define DEFAULT_SIZE 6
#define DEFAULT_AMOUNT 1

int randint(int a, int b) { return a + rand() % (b - a + 1); }

tak_position random_position(int size, int depth, tak_slideslt *slt) {
    tak_position p = tak_new_position(size), np;
    tak_placement p_buffer[200];
    tak_slide s_buffer[500];
    int p_amount, s_amount, n;

    while (--depth > 0) {
        // generate moves
        p_amount = tak_search_placements(p_buffer, &p);
        s_amount = tak_search_slides(s_buffer, &p, slt);
        n = randint(0, p_amount + s_amount - 1);

        // play a random move
        if (n < p_amount) np = tak_do_placement(p, p_buffer[n]);
        else np = tak_do_slide(p, s_buffer[n - p_amount]);

        // check if the game is ended
        if (tak_check_ending(&np).ended) return p;
        else p = np;
    }

    return p;
}

int main(int argc, char **argv) {
    int c, depth_min = DEPTH_MIN, depth_max = DEPTH_MAX, size = DEFAULT_SIZE,
           amount = DEFAULT_AMOUNT;

    while ((c = getopt(argc, argv, "m:M:s:a:")) != -1) {
        switch (c) {
            case 'm': depth_min = atoi(optarg); break;
            case 'M': depth_max = atoi(optarg); break;
            case 's': size = atoi(optarg); break;
            case 'a': amount = atoi(optarg); break;
            case '?': exit(EXIT_FAILURE);
        }
    }

    if (size < 3 || size > 8) size = DEFAULT_SIZE;
    if (depth_min > depth_max) depth_min = depth_max;
    if (amount < 1) amount = 1;

    srand(time(NULL));
    tak_slideslt slt = tak_slt_load();
    char tps[TAK_MAX_TPS];

    for (int i = 0; i < amount; i++) {
        tak_position p = random_position(size, randint(depth_min, depth_max), &slt);
        tak_p2tps(tps, p, TAK_TPS_STD);
        printf("%s\n", tps);
    }

    return EXIT_SUCCESS;
}
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define TAKLIB_IMPLEMENTATION
#include "../lib/ptn.h"
#include "../lib/tak.h"

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

int main() {
    tak_slt slt = tak_slt_fill();
    tak_zobrist_data zd = tak_zd_fill();
    tak_tt *tt = tak_tt_new(10);
    printf("%ld\n", tt->capacity);

    // insert a position
    tak_position p = tps_parse("x2,1,x2/x5/x5/x5/x5 1 1", TPS_STD, &zd);
    tak_tt_set(tt, p);

    // retrieve a position
    tak_position *g = tak_tt_get(tt, p.hash[0]);
    printf("%p: %lu (%lu)\n", g, g->hash[0], p.hash[0]);

    // retrieve a position from a rotated version
    // TODO: test retrieveng from every position?
    tak_position ro = tps_parse("x5/x5/1,x4/x5/x5 1 1", TPS_STD, &zd);
    tak_position *r = tak_tt_search(tt, &ro);
    printf("%p: %lu\n", r, r->hash[0]);

    // fill tt
    srand(time(NULL));
    for (int i = 0; i < 10; i++) tak_tt_set(tt, random_position(5, 10, &slt, &zd));
    printf("%ld\n", tt->overrides);

    // override a position
    tak_position np = random_position(5, 10, &slt, &zd);
    tak_tt_set(tt, np);
    printf("%ld\n", tt->overrides); // should be 2
    printf("%p\n", tak_tt_get(tt, np.hash[0]));

    //
    printf("\nState:\n");
    for (int i = 0; i < tt->capacity; i++) {
        printf("  %d: %p\n", i, tt->items[i]);
    }

    tak_tt_free(tt);
    return EXIT_SUCCESS;
}
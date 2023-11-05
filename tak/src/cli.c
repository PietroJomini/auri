#include <stdbool.h>
#include <stddef.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "moves.h"
#include "notation/tps.h"
#include "position.h"

#define TAK_LOGL 4
#include "lib/log.h"

void tak_ppp(tak_position p) {
    // tps
    char tps[TAK_MAX_TPS];
    tak_p2tps(tps, p, TAK_TPS_STD);
    printf("Position %s\n", tps);

    // top border
    if (p.size > 3) {
        printf("  ┌─colors");
        for (int i = 0; i < p.size * 2 - 6; i++) printf("─");
        printf("┬─mods");
        for (int i = 0; i < p.size * 2 - 4; i++) printf("─");
        printf("┬─heights");
        for (int i = 0; i < p.size * 2 - 7; i++) printf("─");
        printf("┐ ┌─stacks");
        for (int i = 0; i < p.size * 9 - 8; i++) printf("─");
        printf("┐");
    } else {
        // shorter titles on 3x3
        printf("  ┌─col.");
        for (int i = 0; i < p.size * 2 - 4; i++) printf("─");
        printf("┬─mods");
        for (int i = 0; i < p.size * 2 - 4; i++) printf("─");
        printf("┬─heig.");
        for (int i = 0; i < p.size * 2 - 5; i++) printf("─");
        printf("┐ ┌─stacks");
        for (int i = 0; i < p.size * 9 - 8; i++) printf("─");
        printf("┐");
    }

    for (int y = 0; y < p.size; y++) {
        printf("\n%d ┤ ", y + 1);

        // color
        for (int x = 0; x < p.size; x++) {
            uint64_t i = 1ull << (y * p.size + x);
            char c = i & p.cbb[0] ? '0' : i & p.cbb[1] ? '1' : '-';
            printf("%c ", c);
        }
        printf("│ ");

        // wall and caps
        for (int x = 0; x < p.size; x++) {
            uint64_t i = 1ull << (y * p.size + x);
            char c = i & p.walls ? 'w' : i & p.caps ? 'c' : '-';
            printf("%c ", c);
        }
        printf("│");

        // heights
        // will be ugly with heights > 15, but well, i mean...
        for (int x = 0; x < p.size; x++) {
            int i = y * p.size + x;
            printf("%2x", p.heights[i]);
        }
        printf(" │ ");

        // stacks
        // will be ugly with stacks > ff, but well, i mean...
        for (int x = 0; x < p.size; x++) {
            int i = y * p.size + x;
            printf("│ %2d: %2lx ", i, p.stacks[i]);
        }
        printf("│");
    }

    // bottom border
    printf("\n  └─");
    for (int i = 0; i < p.size; i++) printf("┬─");
    printf("┴─");
    for (int i = 0; i < p.size; i++) printf("┬─");
    printf("┴─");
    for (int i = 0; i < p.size; i++) printf("┬─");
    printf("┘ └");
    for (int i = 0; i < p.size * 9 - 1; i++) printf("─");
    printf("┘\n  ");

    // letters
    for (int q = 0; q < 3; q++) {
        printf("  ");
        for (int i = 0; i < p.size; i++) printf("%c ", 'a' + i);
    }

    printf(p.size > 3 ? "    stp=[%d] mc=[%d] F=[%d %d] C=[%d %d]\n"
                      : "    stp=[%d] mc=[%d]\n                              F=[%d "
                        "%d] C=[%d %d]\n",
           p.stp, p.mc, p.reserves[0][0], p.reserves[1][0], p.reserves[0][1],
           p.reserves[1][1]);
}

void tak_perftd(tak_position p, int depth) {
    uint64_t nodes = 0, nnodes = 0;
    tak_slideslt slt = tak_slt_load();

    // load moves
    tak_placement pb[196];
    tak_slide sb[500];

    int np = tak_search_placements(pb, &p);
    int ns = tak_search_slides(sb, &p, &slt);

    for (int i = 0; i < np; i++) {
        nnodes = tak_perft(tak_do_placement(p, pb[i]), depth - 1, &slt);
        printf("Placement %d %d: %ld\n", pb[i].at, pb[i].piece, nnodes);
        nodes += nnodes;
    }
    for (int i = 0; i < ns; i++) {
        nnodes = tak_perft(tak_do_slide(p, sb[i]), depth - 1, &slt);
        printf("Placement %d %d %d: %ld\n", sb[i].origin, sb[i].length, sb[i].stacks,
               nnodes);
        nodes += nnodes;
    }

    printf("nodes: %ld\n", nodes);
}
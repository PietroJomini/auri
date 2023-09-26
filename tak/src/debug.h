/*
dummy debug utils, supposed to be removed in the future
*/

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "moves.h"
#include "position.h"
#include "ptn.h"

// pretty print a bitboard
void ppbb(uint64_t bb, uint8_t size) {
    printf("BB[%#016lx]\n", bb);
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            int index = row * size + col;
            printf((bb & SQ(index)) ? "# " : "- ");
        }
        printf("\n");
    }
}

// pretty print position
void ppp(Position p) {
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
            char c = i & p.white ? '0' : i & p.black ? '1' : '-';
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

    printf("\n");
}

// perft divide
void perftd(Position p, int depth) {
    Placement pb[196];
    Slide sb[200];

    int np = placements(pb, p);
    int ns = slides(sb, p);
    int nodes = 0;
    char ptn[MAX_PTN_MOVE_S];

    for (int i = 0; i < np; i++) {
        int dnodes = perft(do_placement(p, pb[i]), depth - 1);
        placement2ptn(ptn, pb[i], p);
        printf("%s %d\n", ptn, dnodes);
        nodes += dnodes;
    }

    for (int i = 0; i < ns; i++) {
        int dnodes = perft(do_slide(p, sb[i]), depth - 1);
        slide2ptn(ptn, sb[i], p);
        printf("%s %d\n", ptn, dnodes);
        nodes += dnodes;
    }

    printf("%d\n", nodes);
}

// bits string to long
inline uint64_t bs2l(char *str) { return (uint64_t)strtol(str, NULL, 2); }
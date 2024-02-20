/*
- [x] tui board representation

guard macros
- TAKLIB_INCLUDE_UI: include guard
- TAKLIB_IMPLEMENTATION: implementation guard
*/

#ifndef TAKLIB_INCLUDE_UI
#define TAKLIB_INCLUDE_UI

#include <stdint.h>

#ifdef TAKLIB_IMPLEMENTATION
#undef TAKLIB_IMPLEMENTATION
#include "tak.h"
#include "ptn.h"
#define TAKLIB_IMPLEMENTATION
#else
#include "tak.h"
#include "ptn.h"
#endif

// print a representation of a position
void tak_print(tak_position p);

#endif  // TAKLIB_INCLUDE_UI

// implementations
#ifdef TAKLIB_IMPLEMENTATION
// NOLINTBEGIN(misc-definitions-in-headers)

// TODO: this is messy, refactor
void tak_print(tak_position p) {
    // tps
    char tps[TPS_MAX_LENGTH];
    tps_encode(tps, p, TPS_STD);
    printf("Position [%s]\n", tps);

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

// NOLINTEND(misc-definitions-in-headers)
#endif  // TAKLIB_IMPLEMENTATION
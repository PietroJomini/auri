#include "notation.h"

#include "tak.h"

void ppp(position p) {
    // tps
    char tps[TPS_MAX_LENGTH];
    position2tps(tps, p, TPS_STD);
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

const tps_lexicon TPS_STD = {
    .row_s = '/',
    .cell_s = ',',
    .section_s = ' ',
    .wall = 'S',
    .cap = 'C',
    .jump = 'x',
    .white = '1',
    .black = '2',
};

int tps_getsize(char *tps, tps_lexicon lexicon) {
    int size = 1;
    while (*tps != lexicon.section_s) size += lexicon.row_s == *tps++;
    return size;
}

int mirror_index(int index, int size) {
    return index % size + (size - index / size - 1) * size;
}

// TODO: benchmark against a strtok_r version
position tps2position(char *tps, tps_lexicon lexicon) {
    position p = new_position(tps_getsize(tps, lexicon));

    // main section
    player color = WHITE;
    int index = 0, mindex = mirror_index(0, p.size), jumping = 0;
    u64 square = 0;

    for (; *tps != lexicon.section_s; tps += sizeof(char)) {
        // handle empty cell
        if (*tps == lexicon.jump) jumping = 1;

        // handle multiple empty cells
        else if (jumping && *tps >= '0' && *tps <= '9') jumping = *tps - '0';

        // handle color indicator (hence stone)
        // we don't handle previous jump here, since there shouldn't be the need to
        // as after a jump indicator there should be only a number (handled above)
        // or a separator
        else if (*tps == lexicon.white || *tps == lexicon.black) {
            color = *tps == lexicon.white ? WHITE : BLACK;
            mindex = mirror_index(index, p.size);

            // update stacks
            p.stacks[mindex] = (p.stacks[mindex] << 1) | color;
            p.heights[mindex] += 1;

            // update color bitboards
            square = 1ull << mindex;
            p.cbb[color] |= square;
            p.cbb[1 - color] &= ~square;

            // update flats count
            p.reserves[color][0] -= 1;
        }

        // with separators and modifiers we need to handle
        // unhandled jumps as well
        else {
            // resolve jumps
            if (jumping) {
                index += jumping - 1;
                jumping = 0;
            }

            // resolve modifiers
            // here the mindex should already be set, since all modifiers
            // should be after a color indicator
            if (*tps == lexicon.wall) p.walls |= 1ull << mindex;
            else if (*tps == lexicon.cap) {
                p.caps |= 1ull << mindex;

                // update stones count
                p.reserves[color][1] -= 1;
                p.reserves[color][0] += 1;
            }

            // update index when we find a separator
            else if (*tps == lexicon.cell_s || *tps == lexicon.row_s)
                index += 1;
        }
    }

    // turn identifier
    while (*tps == lexicon.section_s) tps++;
    p.stp = strtol(tps, &tps, 10) - 1;

    // size to play section
    while (*tps == lexicon.section_s) tps++;
    p.mc = strtol(tps, &tps, 10) - 1;
    p.mc = p.mc * 2 + p.stp;

    return p;
}

int position2tps(char *buffer, position p, tps_lexicon lexicon) {
    int k = 0;

    // main section
    int jumping = 0;

    for (int row = p.size - 1; row >= 0; row--) {
        for (int column = 0; column < p.size; column++) {
            int index = row * p.size + column;

            if (p.heights[index] == 0) jumping += 1;
            else {
                // handle previous jumps
                if (jumping) {
                    // add cell separator if needed
                    if (k > 0 && buffer[k - 1] != lexicon.row_s) {
                        buffer[k++] = lexicon.cell_s;
                    }

                    buffer[k++] = lexicon.jump;

                    // add jumps count if needed
                    if (jumping > 1) buffer[k++] = '0' + jumping;
                    jumping = 0;
                }

                // cell separator
                if (column > 0) buffer[k++] = lexicon.cell_s;

                // append stack
                uint64_t sq;  // this could be a int, but later it's used as a uint64
                for (int h = p.heights[index] - 1; h >= 0; h--) {
                    sq = p.stacks[index] & 1ull << h;
                    buffer[k++] = sq ? lexicon.black : lexicon.white;
                }

                // modifiers
                sq = 1ull << index;
                if (sq & p.walls) buffer[k++] = lexicon.wall;
                if (sq & p.caps) buffer[k++] = lexicon.cap;
            }
        }

        // handle remaining jumps
        if (jumping) {
            // if we jump for less than ths full row, we can be sure that there
            // were cells previously, so we need the separator
            if (jumping < p.size) buffer[k++] = ',';

            buffer[k++] = lexicon.jump;
            if (jumping > 1) buffer[k++] = '0' + jumping;
            jumping = 0;
        }

        // row separator
        if (row > 0) buffer[k++] = lexicon.row_s;
    }

    // move count sections
    int rv = sprintf(buffer + k, "%c%d%c%d", lexicon.section_s, p.stp + 1,
                     lexicon.section_s, p.mc / 2 + 1);
    return rv < 0 ? rv : k + rv;
}

const ptn_lexicon PTN_STD = {
    .wall = 'S',
    .cap = 'C',
    .columns = {'1', '2', '3', '4', '5', '6', '7', '8'},
    .rows = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'},
    .directions = {'+', '>', '-', '<'},  // north, east, south, west
    .flattens = '*',
};

int placement2ptn(char *buffer, placement p, u8 size, ptn_lexicon lexicon) {
    int k = 0;

    // piece modifiers
    if (p.piece & WALL) buffer[k++] = lexicon.wall;
    if (p.piece & CAP) buffer[k++] = lexicon.cap;

    // square
    buffer[k++] = lexicon.rows[p.at % size];
    buffer[k++] = lexicon.columns[p.at / size];

    buffer[k++] = '\0';
    return k;
}

int slide2ptn(char *buffer, slide s, u8 size, ptn_lexicon lexicon) {
    int k = 0;

    // count
    // TODO: if the count is > 9 this will fail
    int count = 0;
    for (int i = 0; i < s.length; i++) count += (s.stacks >> 4 * i) & 0xf;
    if (count > 1) buffer[k++] = '0' + count;

    // square, direction
    buffer[k++] = lexicon.rows[s.origin % size];
    buffer[k++] = lexicon.columns[s.origin / size];
    buffer[k++] = lexicon.directions[s.direction];

    // drop count
    if (s.length > 1)
        for (int i = 0; i < s.length; i++) buffer[k++] = '0' + (s.stacks >> 4 * i & 0xf);

    // flattens
    if (s.flattens) buffer[k++] = lexicon.flattens;

    buffer[k++] = '\0';

    return k;
}

int move2ptn(char *buffer, move m, u8 size, ptn_lexicon lexicon) {
    switch (m.t) {
        case PLACEMENT: return placement2ptn(buffer, m.p, size, lexicon);
        case SLIDE: return slide2ptn(buffer, m.s, size, lexicon);
    }

    return 0;
}
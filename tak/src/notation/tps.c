#include "tps.h"

#include <stdio.h>
#include <stdlib.h>

tak_tps_lexicon const TAK_TPS_STD = {
    .row_s = '/',
    .cell_s = ',',
    .section_s = ' ',
    .wall = 'S',
    .cap = 'C',
    .jump = 'x',
    .white = '1',
    .black = '2',
};

int tak_tps_gets(char *tps, tak_tps_lexicon lexicon) {
    int size = 1;
    while (*tps != lexicon.section_s) size += lexicon.row_s == *tps++;
    return size;
}

int tak_mirror_index(int index, int size) {
    return index % size + (size - index / size - 1) * size;
}

// TODO: benchmark against a strtok_r version
tak_position tak_tps2p(char *tps, tak_tps_lexicon lexicon) {
    tak_position p = tak_new_position(tak_tps_gets(tps, lexicon));

    // main section
    tak_player color = TAK_WHITE;
    int index = 0, mindex = tak_mirror_index(0, p.size), jumping = 0;
    uint64_t square = 0;

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
            color = *tps == lexicon.white ? TAK_WHITE : TAK_BLACK;
            mindex = tak_mirror_index(index, p.size);

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

int tak_p2tps(char *buffer, tak_position p, tak_tps_lexicon lexicon) {
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

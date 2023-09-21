#include "tps.h"

#include <stdlib.h>

#include "position.h"

Position tps2p(char *tps) {
    Position p = pempty();
    p.size = 1;

    // pieces used
    int flats[2] = {0, 0};
    int caps[2] = {0, 0};

    // board description
    int value = 0;
    int index = 0;
    int jumping = 0;
    while (*tps != ' ') {
        if (*tps == 'x') jumping = 1;
        else if ('0' <= *tps && *tps <= '9') {
            // here only digits, so it's safe to convert abruptly
            value = *tps - '0';

            // increment jumps
            if (jumping) jumping = value;
            else {
                // update stacks
                p.stacks[index] |= (value - 1) << p.heights[index];
                p.heights[index] += 1;

                // update wb bitboards
                uint64_t *bb = (value == 1) ? &p.white : &p.black;
                uint64_t *obb = (value == 1) ? &p.black : &p.white;
                *bb |= SQ(index);
                *obb &= ~SQ(index);

                // update flats count
                flats[value - 1] += 1;
            }
        } else {
            // here remains separators and modifiers
            // cell separators can be ignored, as long as we resolve jumps

            // resolve jumps
            if (jumping) {
                index += jumping - 1;
                jumping = 0;
            };

            // resolve row separator
            if (*tps == '/') p.size += 1;

            // resolve modifiers
            if (*tps == 'S') p.walls |= SQ(index);
            if (*tps == 'C') {
                p.caps |= SQ(index);

                // update caps count
                caps[value - 1] += 1;
                flats[value - 1] -= 1;
            }

            // update index if we are at a separator
            if (*tps == '/' || *tps == ',') index += 1;
        }

        tps += sizeof(char);
    }

    // turn identifier + move counter
    p.stp = strtol(tps, &tps, 10) - 1;
    p.mc = strtol(tps, NULL, 10) - 1;

    // in the std format rows are listed from the "top" one,
    // but it makes sense to let the indexes start from the
    // bottom left. eg, in a 3x3, tps ordering is
    //      1 2 3
    //      4 5 6
    //      7 8 9
    // that get pushed in a bitboard that results in
    //      0b... b b b b b b b b b
    //      index 9 8 7 6 5 4 3 2 1
    // but in-memory the same bitboard should translate to
    //      7 8 9
    //      4 5 6
    //      1 2 3
    // so we have to reverse chunks of p.size bits.
    // i could probably so this with some index stuff while
    // parsing, but this is cleaner and i will need to rotate
    // bitboards later anyway
    int shift = 64 - p.size * p.size;
    p.white = rotate(p.white, p.size) >> shift;
    p.black = rotate(p.black, p.size) >> shift;
    p.caps = rotate(p.caps, p.size) >> shift;
    p.walls = rotate(p.walls, p.size) >> shift;

    // thus i nees to rotate the heights and stacks as well
    // TODO: maybe now it's better to do this in the read loop
    //       with some index messing...
    uint8_t th, d;
    uint64_t ts;
    for (int i = 0; i < p.size * (p.size / 2); i++) {
        // cell displacement simmetry
        d = p.size * (p.size - i / p.size * 2 - 1);

        // swap heights
        th = p.heights[i];
        p.heights[i] = p.heights[i + d];
        p.heights[i + d] = th;

        // swap stacks
        ts = p.stacks[i];
        p.stacks[i] = p.stacks[i + d];
        p.heights[i + d] = ts;
    }

    // setup remaining pieces, given the board size
    p.reserve[0][0] = SETUP[p.size - 3][0] - flats[0];
    p.reserve[1][0] = SETUP[p.size - 3][0] - flats[1];
    p.reserve[0][1] = SETUP[p.size - 3][1] - caps[0];
    p.reserve[1][1] = SETUP[p.size - 3][1] - caps[1];

    return p;
}
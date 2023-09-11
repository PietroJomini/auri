#include "tps.h"

#include <stdlib.h>
#include <string.h>

#include "position.h"

Position tps2p(char *tps) {
    Position p = pempty();
    p.size = 1;

    // board description
    int index = 0;
    int jumping = 0;
    while (*tps != ' ') {
        if (*tps == 'x') jumping = 1;
        else if ('0' <= *tps && *tps <= '9') {
            // here only digits, so it's safe to convert abruptly
            int value = *tps - '0';

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
            if (*tps == 'C') p.caps |= SQ(index);

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

    return p;
}
/*
dummy debug utils, supposed to be removed in the future
*/

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "position.h"

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

// bits string to long
inline uint64_t bs2l(char *str) { return (uint64_t)strtol(str, NULL, 2); }
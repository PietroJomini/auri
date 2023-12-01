// shamelessly copied from https://github.com/ViliamVadocz/tak/blob/main/tak/tests/perft.rs

#include <assert.h>

#include "../src/tak.h"

void test_5(slides_lt *slt, zobrist_data *zd) {
    assert(perft(new_position(5), 0, slt, zd) == 1);
    assert(perft(new_position(5), 1, slt, zd) == 25);
    assert(perft(new_position(5), 2, slt, zd) == 600);
    assert(perft(new_position(5), 3, slt, zd) == 43320);
    assert(perft(new_position(5), 4, slt, zd) == 2999784);
}

void test_6(slides_lt *slt, zobrist_data *zd) {
    assert(perft(new_position(6), 0, slt, zd) == 1);
    assert(perft(new_position(6), 1, slt, zd) == 36);
    assert(perft(new_position(6), 2, slt, zd) == 1260);
    assert(perft(new_position(6), 3, slt, zd) == 132720);
    assert(perft(new_position(6), 4, slt, zd) == 13586048);
}

int main() {
    slides_lt slt = slt_fill();
    zobrist_data zd = zobrist_fill();

    test_5(&slt, &zd);
    test_6(&slt, &zd);

    return 0;
}

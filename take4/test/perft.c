// shamelessly copied from https://github.com/ViliamVadocz/tak/blob/main/tak/tests/perft.rs

#include <assert.h>

#include "../src/tak.h"

void test_5(slides_lt *slt) {
    assert(perft(new_position(5), 0, slt) == 1);
    assert(perft(new_position(5), 1, slt) == 25);
    assert(perft(new_position(5), 2, slt) == 600);
    assert(perft(new_position(5), 3, slt) == 43320);
    assert(perft(new_position(5), 4, slt) == 2999784);
}

void test_6(slides_lt *slt) {
    assert(perft(new_position(6), 0, slt) == 1);
    assert(perft(new_position(6), 1, slt) == 36);
    assert(perft(new_position(6), 2, slt) == 1260);
    assert(perft(new_position(6), 3, slt) == 132720);
    assert(perft(new_position(6), 4, slt) == 13586048);
}

int main() {
    slides_lt slt = slt_fill();

    test_5(&slt);
    test_6(&slt);

    return 0;
}

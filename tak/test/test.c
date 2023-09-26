// clang-format off

#include <assert.h>
#include <stdlib.h>
#include "../src/auri.h"

// TODO: more manually crafted test positions
// TODO: test stacks
// TODO: test heights
// TODO: test remaining pieces
void test_tps() {
    Position p1 = tps2p("x5/x5/x5/x5/x5 1 1");
    assert(p1.white == 0);
    assert(p1.black == 0);
    assert(p1.caps  == 0);
    assert(p1.walls == 0);
    assert(p1.mc    == 0);
    assert(p1.stp   == 0);
    assert(p1.size  == 5);

    Position p2 = tps2p("x3,12,2S/x,22S,22C,11,21/121,212,12,1121C,1212S/21S,1,21,211S,12S/x,21S,2,x2 1 26");
    assert(p2.white == bs2l("0000011000010010111100010"));
    assert(p2.black == bs2l("1100000110101101000000100"));
    assert(p2.walls == bs2l("1000000010100001100100010"));
    assert(p2.caps  == bs2l("0000000100010000000000000"));
    assert(p2.size  == 5 && p2.stp == 0 && p2.mc == 25);

    Position p3 = tps2p("x,12,2,x,1,2S,1/2,2,x,122221,x,1,112S/1,2,1,x,2,1,2/1,1,222221C,2S,111112S,2,1/1,221S,x,112,1112C,221S,1111112C/222221C,221,x2,1,2,2/2,2,1,1,21,x,112 1 86");
    assert(p3.white == bs2l("1010000010100001001011000111010001100100110011100"));
    assert(p3.black == bs2l("0100110100001110100100111000101100011000001000011"));
    assert(p3.walls == bs2l("0100000100000000000000011000010001000000000000000"));
    assert(p3.caps  == bs2l("0000000000000000000000000100101000000000010000000"));
    assert(p3.size  == 7 && p3.stp == 0 && p3.mc == 85);
}

// TODO: test placements (do)
// TODO: test slides (do)
// TODO: share data between tests
// TODO: test ptn
int main() {
    test_tps();
    return 0;
}
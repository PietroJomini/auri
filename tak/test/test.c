// clang-format off

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../src/auri.h"

#define ASSERT_STR_EQ(s1, s2) assert(!strcmp(s1, s2))

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
    assert(p2.size  == 5 && p2.stp == 0 && p2.mc == 12);

    Position p3 = tps2p("x,12,2,x,1,2S,1/2,2,x,122221,x,1,112S/1,2,1,x,2,1,2/1,1,222221C,2S,111112S,2,1/1,221S,x,112,1112C,221S,1111112C/222221C,221,x2,1,2,2/2,2,1,1,21,x,112 1 86");
    assert(p3.white == bs2l("1010000010100001001011000111010001100100110011100"));
    assert(p3.black == bs2l("0100110100001110100100111000101100011000001000011"));
    assert(p3.walls == bs2l("0100000100000000000000011000010001000000000000000"));
    assert(p3.caps  == bs2l("0000000000000000000000000100101000000000010000000"));
    assert(p3.size  == 7 && p3.stp == 0 && p3.mc == 42);
}

void test_playtak2position() {
    char tps[500];

    // ID 551597
    p2tps(tps, playtak2position("P A5,P C1,P C3,P B5,P C2,P C5,P C4 C,P D5,P E5,P E4,M E5 D5 1,M E4 D4 1,M D5 C5 2", 5));
    ASSERT_STR_EQ(tps, "2,2,221,x2/x2,1C,2,x/x2,1,x2/x2,1,x2/x2,1,x2 2 7");

    // TODO: this times out? wut?
    // ID 559874
    p2tps(tps, playtak2position("P E6,P F6,P D4,P E4 C,P E3 C,P F5,P D3,P D5,P C3,P C5,P B3,P C2 W,P F3,M C2 C3 1,P C4,P B4,P A4,M E4 D4 1,P B5,P A5,P E5 W,P D6,P B6,P E4,P F4,P A3,P B2,M B4 B5 1,M E5 F5 1,P B4,P A2,P C2,P C1,P B1 W,P D2,M B1 B2 1,P B1,P D1,P A1,P F2,P E1,M F2 F3 1,M E3 F3 1,P E5,P A6 W,P E2,P E3,M C2 D2 1,M D3 D2 1,M D4 D2 1 1,P D4,M D2 D3 2,M E1 D1 1,P E1,M E3 E2 1,M D2 E2 2,M F3 E3 2,M E2 D2 3,M E3 E2 2,P E3,M D1 D2 2,P F2,M A6 A5 1,P F1,M F3 F2 1,M D3 D2 3,M A5 B5 2,P D1,P D3 W,P A5,P A6,P F3,M F5 E5 2,P F5,M B5 D5 1 3,P B5,P C6,M C3 C4 2,P C3,P C2", 6));
    ASSERT_STR_EQ(tps, "1,1,1,2,2,1/2,2,21,2221S,221S,2/1,2,112S,1,2,1/2,1,1,1S,2,2/1,12S,2,11221112C,221C,21/1,1,1,2,2,2 1 41");

    // ID 187210
    p2tps(tps, playtak2position("P A5,P E1,P B1,P A4,P A1,P C1,P C2 C,P A3,P B2,P A2,P D1,P D2 W,M C2 C1 1", 5));
    ASSERT_STR_EQ(tps, "2,x4/2,x4/2,x4/2,1,x,2S,x/1,1,21C,1,1 2 7");

    // ID 94203
    p2tps(tps, playtak2position("P F6,P F1,P D4,P E4,P D3,P D5,P E3,P C5,P C4 C,P B5,P B4,P A4,P F3,P A5,P A3,M A4 B4 1,P B3,P C3 C,M B3 B4 1,M C3 D3 1,M B4 B5 2,P B6,P A4,P E5,M E3 E4 1,M D3 D4 2,P D3,P E6,M B5 C5 3,P B5,P C3,P E3 W,P D2,P C6,M C5 A5 1 3,P A6,M A5 A6 4,P A5,M A4 A5 1,M B5 A5 1,M A6 A5 5,M B5 A5 1,P B5 W,P A6,M E4 E6 1 1,M A5 A1 1 1 1 3", 6));
    ASSERT_STR_EQ(tps, "2,2,2,x,21,2/212,1S,x,2,22,x/2,1,1C,112C,x2/12,x,1,1,2S,1/2,x2,1,x2/212,x4,1 1 24");

    // ID 179723
    p2tps(tps, playtak2position("P A5,P A1,P C3,P B2,P C2,P C1 C,P B3,P A3,P A2,P B1,P B4,P A4,M C2 B2 1,P B5,P C4,P C5,P D4,M C5 C4 1,P D3,M C4 C3 2,P C2 C,P D2,P C4,M B1 B2 1,M C2 B2 1,M A3 B3 1,M B2 B3 2,M C3 C4 3,P C3,M D2 D3 1,M D4 D3 1,P E3 W,M B4 C4 1,P D4 W,M C4 C5 4", 5));
    ASSERT_STR_EQ(tps, "2,2,1121,x2/2,x,1,2S,x/x,1221C,1,121,2S/1,21,x3/1,x,2C,x2 2 18");
}

// TODO: test placements (do)
// TODO: test slides (do)
// TODO: share data between tests
// TODO: test ptn
// TODO: test game endings
int main() {
    test_tps();
    test_playtak2position();
    return 0;
}
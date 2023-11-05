#include <criterion/assert.h>
#include <criterion/criterion.h>
#include <criterion/internal/test.h>
#include <criterion/logging.h>
#include <criterion/parameterized.h>

#include "../src/notation/tps.h"
#include "../src/position.h"

// == -- helpers -- == //

// bit string to long
uint64_t bs2l(char *str) { return (uint64_t)strtol(str, NULL, 2); }

// == -- ####### -- == //

// == -- custom asserts -- == //

// assert equality of a position
void assert_eqp(tak_position a, tak_position b) {
    cr_assert_eq(a.size, b.size);
    cr_assert_eq(a.stp, b.stp);
    cr_assert_eq(a.mc, b.mc);
    cr_assert_eq(a.cbb[0], b.cbb[0]);
    cr_assert_eq(a.cbb[1], b.cbb[1]);
    cr_assert_eq(a.caps, b.caps);
    cr_assert_eq(a.walls, b.walls);
    cr_assert_eq(a.reserves[0][0], b.reserves[0][0]);
    cr_assert_eq(a.reserves[0][1], b.reserves[0][1]);
    cr_assert_eq(a.reserves[1][0], b.reserves[1][0]);
    cr_assert_eq(a.reserves[1][1], b.reserves[1][1]);
    for (int i = 0; i < 64; i++) {
        cr_assert_eq(a.heights[i], b.heights[i],
                     "heights differs: \n\tindex: %d \n\ta: %d \n\tb: %d", i,
                     a.heights[i], b.heights[i]);
        cr_assert_eq(a.stacks[i], b.stacks[i],
                     "stack differs: \n\tindex: %d \n\ta: 0x%lx \n\tb: 0x%lx", i,
                     a.stacks[i], b.stacks[i]);
    }
}

// == -- ############## -- == //

Test(position, empty) {
    assert_eqp(
        tak_new_position(5),
        (tak_position){.stp = TAK_WHITE, .size = 5, .reserves = {{21, 1}, {21, 1}}});
    assert_eqp(
        tak_new_position(8),
        (tak_position){.stp = TAK_WHITE, .size = 8, .reserves = {{50, 2}, {50, 2}}});
}

Test(ptn, tps) {
    // clang-format off
    char tps[3][TAK_MAX_TPS] = {
        "x5/x5/x5/x5/x5 1 1",
        "x3,12,2S/x,22S,22C,11,21/121,212,12,1121,1212S/21S,1,21,211S,12S/x,21S,2,x2 1 26",
        "x,12,2,x,1,2S,1/2,2,x,122221,x,1,112S/1,2,1,x,2,1,2/1,1,222221C,2S,111112S,2,1/1,221S,x,112,1112C,221S,1111112C/222221C,221,x2,1,2,2/2,2,1,1,21,x,112 1 86",
    };
    // clang-format on

    // parse positions
    tak_position positions[3];
    for (int i = 0; i < 3; i++) positions[i] = tak_tps2p(tps[i], TAK_TPS_STD);

    // assert positions
    assert_eqp(positions[0], tak_new_position(5));
    assert_eqp(
        positions[1],
        (tak_position){
            .size = 5,
            .stp = TAK_WHITE,
            .mc = 50,
            .cbb[0] = bs2l("0000011000010010111100010"),
            .cbb[1] = bs2l("1100000110101101000000100"),
            .walls = bs2l("1000000010100001100100010"),
            .caps = bs2l("0000000100000000000000000"),
            .stacks = {bs2l("0"),   bs2l("10"),  bs2l("1"),  bs2l("0"),    bs2l("0"),
                       bs2l("10"),  bs2l("0"),   bs2l("10"), bs2l("100"),  bs2l("01"),
                       bs2l("010"), bs2l("101"), bs2l("01"), bs2l("0010"), bs2l("0101"),
                       bs2l("0"),   bs2l("11"),  bs2l("11"), bs2l("0"),    bs2l("10"),
                       bs2l("0"),   bs2l("0"),   bs2l("0"),  bs2l("01"),   bs2l("1")},
            .heights = {0, 2, 1, 0, 0, 2, 1, 2, 3, 2, 3, 3, 2,
                        4, 4, 0, 2, 2, 2, 2, 0, 0, 0, 2, 1},
            .reserves = {{1, 1}, {2, 0}},
        });
    assert_eqp(
        positions[2],
        (tak_position){
            .size = 7,
            .stp = TAK_WHITE,
            .mc = 170,
            .cbb[0] = bs2l("1010000010100001001011000111010001100100110011100"),
            .cbb[1] = bs2l("0100110100001110100100111000101100011000001000011"),
            .walls = bs2l("0100000100000000000000011000010001000000000000000"),
            .caps = bs2l("0000000000000000000000000100101000000000010000000"),
            .stacks = {bs2l("1"),       bs2l("1"),      bs2l("0"),      bs2l("0"),
                       bs2l("10"),      bs2l("0"),      bs2l("001"),    bs2l("111110"),
                       bs2l("110"),     bs2l("0"),      bs2l("0"),      bs2l("0"),
                       bs2l("1"),       bs2l("1"),      bs2l("0"),      bs2l("110"),
                       bs2l("0"),       bs2l("001"),    bs2l("0001"),   bs2l("110"),
                       bs2l("0000001"), bs2l("0"),      bs2l("0"),      bs2l("111110"),
                       bs2l("1"),       bs2l("000001"), bs2l("1"),      bs2l("0"),
                       bs2l("0"),       bs2l("1"),      bs2l("0"),      bs2l("0"),
                       bs2l("1"),       bs2l("0"),      bs2l("1"),      bs2l("1"),
                       bs2l("1"),       bs2l("0"),      bs2l("011110"), bs2l("0"),
                       bs2l("0"),       bs2l("001"),    bs2l("0"),      bs2l("01"),
                       bs2l("1"),       bs2l("0"),      bs2l("0"),      bs2l("1"),
                       bs2l("0")},
            .heights = {1, 1, 1, 1, 2, 0, 3, 6, 3, 0, 0, 1, 1, 1, 1, 3, 0,
                        3, 4, 3, 7, 1, 1, 6, 1, 6, 1, 1, 1, 1, 1, 0, 1, 1,
                        1, 1, 1, 0, 6, 0, 1, 3, 0, 2, 1, 0, 1, 1, 1},
            .reserves = {{0, 0}, {1, 0}},
        });

    // assert tps encoding
    char ntps[TAK_MAX_TPS] = {0};
    for (int i = 0; i < 3; i++) {
        cr_assert_gt(tak_p2tps(ntps, tak_tps2p(tps[i], TAK_TPS_STD), TAK_TPS_STD), 0);
        cr_assert_str_eq(tps[i], ntps);
    }
}
// shamelessly stolen from
// https://github.com/ViliamVadocz/tak/blob/main/tak/tests/perft.rs

#include <stdio.h>

// #define TAKLIB_NOHASH
#define TAKLIB_IMPLEMENTATION
#include "../lib/ptn.h"
#include "../lib/tak.h"
#include "../lib/ui.h"

#define assert(cond)                                                              \
    ({                                                                            \
        const int __res = cond;                                                   \
        if (!__res)                                                               \
            fprintf(stderr, "[%s:%d] assertion failed: %s\n", __FILE__, __LINE__, \
                    #cond);                                                       \
    });

#define assert_eu64(a, b)                                                          \
    ({                                                                             \
        tak_u64 __a = (a);                                                         \
        tak_u64 __b = (b);                                                         \
        if (__a != __b)                                                            \
            fprintf(stderr, "[%s:%s:%d] assertion failed: %s(=%ld) != %s(=%ld)\n", \
                    __FILE__, __FUNCTION__, __LINE__, #a, __a, #b, __b);           \
    })

// this kinda ugly, bu at the same time kinda useful... should it be somewhere in lib?
#define pos(size, zd, ...)                                         \
    (ptn_apply(tak_newposition(size), (char *[]){__VA_ARGS__}, \
               sizeof((char *[]){__VA_ARGS__}) / sizeof(char *), PTN_STD, zd))

void test_5(tak_slt *slt, tak_zobrist_data *zd) {
    tak_position p = tak_newposition(5);
    assert_eu64(tak_perft(p, 0, slt, zd), 1);
    assert_eu64(tak_perft(p, 1, slt, zd), 25);
    assert_eu64(tak_perft(p, 2, slt, zd), 600);
    assert_eu64(tak_perft(p, 3, slt, zd), 43320);
    assert_eu64(tak_perft(p, 4, slt, zd), 2999784);
}

void test_6(tak_slt *slt, tak_zobrist_data *zd) {
    tak_position p = tak_newposition(6);
    assert_eu64(tak_perft(p, 0, slt, zd), 1);
    assert_eu64(tak_perft(p, 1, slt, zd), 36);
    assert_eu64(tak_perft(p, 2, slt, zd), 1260);
    assert_eu64(tak_perft(p, 3, slt, zd), 132720);
    assert_eu64(tak_perft(p, 4, slt, zd), 13586048);
}

void test_stack(tak_slt *slt, tak_zobrist_data *zd) {
    tak_position p = pos(5, zd, "d3", "c3", "c4", "1d3<", "1c4-", "Sc4");

    assert_eu64(tak_perft(p, 0, slt, zd), 1);
    assert_eu64(tak_perft(p, 1, slt, zd), 87);
    assert_eu64(tak_perft(p, 2, slt, zd), 6155);
    assert_eu64(tak_perft(p, 3, slt, zd), 461800);
}

void test_carry_limit(tak_slt *slt, tak_zobrist_data *zd) {
    tak_position p = pos(5, zd, "c2", "c3", "d3", "b3", "c4", "1c2+", "1d3<", "1b3>", "1c4-",
                         "Cc2", "a1", "1c2+", "a2");

    assert_eu64(tak_perft(p, 0, slt, zd), 1);
    assert_eu64(tak_perft(p, 1, slt, zd), 104);
    assert_eu64(tak_perft(p, 2, slt, zd), 7743);
    assert_eu64(tak_perft(p, 3, slt, zd), 592645);
}

void test_suicide(tak_slt *slt, tak_zobrist_data *zd) {
    // x2,2,x,2/x,12,x,12,x/212,x2,2121C,x/x,1,112221,1,1/x5 2 21
    tak_position p =
        pos(5, zd, "c4", "c2", "d2", "c3", "b2", "d3", "1d2+", "b3", "d2", "b4", "1c2+",
            "1b3>", "2d3<", "1c4-", "d4", "5c3<23", "c2", "c4", "1d4<", "d3", "1d2+",
            "1c3+", "Cc3", "2c4>", "1c3<", "d2", "c3", "1d2+", "1c3+", "1b4>", "2b3>11",
            "3c4-12", "d2", "c4", "b4", "c5", "1b3>", "1c4<", "3c3-", "e5", "e2");

    assert_eu64(tak_perft(p, 0, slt, zd), 1);
    assert_eu64(tak_perft(p, 1, slt, zd), 85);
    assert_eu64(tak_perft(p, 2, slt, zd), 11206);
    assert_eu64(tak_perft(p, 3, slt, zd), 957000);
}

void test_endgame(tak_slt *slt, tak_zobrist_data *zd) {
    // 2,2C,1,1,1/1,1,2,21C,x/1,2121,1,2,2/2,2,1,22112S,2/2,1S,2,2,1 1 21
    // 2,2C,1,1,1/1,1,2,21C,x/1,2121,1,2,2/2,2,1,22112,2/2,1S,2,2,1 1 21
    tak_position p = pos(5, zd, "a5", "b4", "c3", "d2", "e1", "d1", "c2", "d3", "c1", "d4",
                         "d5", "c4", "c5", "b3", "b2", "a2", "Sb1", "a3", "Ce4", "Cb5",
                         "a4", "a1", "e5", "e3", "c3<", "Sc3", "c1>", "c1", "2d1+", "c3-",
                         "c3", "a3>", "a3", "d1", "e4<", "2c2>", "c2", "e2", "b2+", "b2");

    assert_eu64(tak_perft(p, 0, slt, zd), 1);
    assert_eu64(tak_perft(p, 1, slt, zd), 65);
    assert_eu64(tak_perft(p, 2, slt, zd), 4072);
    assert_eu64(tak_perft(p, 3, slt, zd), 272031);
    assert_eu64(tak_perft(p, 4, slt, zd), 16642760);
}

void test_reserves(tak_slt *slt, tak_zobrist_data *zd) {
    // 1,x,2,2,x/1,221,2,21,x/x,1,12,211221,122S/21,12112,1,2212,2/2,1,1,1,1 2 30
    tak_position p = pos(
        5, zd, "a1", "b1", "c1", "d1", "e1", "e2", "d2", "c2", "b2", "a2", "a3", "b3", "c3",
        "d3", "e3", "a4", "b4", "c4", "d4", "e4", "a5", "a4-", "b4-", "c4-", "d4-", "e4-",
        "a4", "b4", "c4", "d4", "e4", "2a3>", "c4>", "2e3<", "a3", "4b3-", "b3", "c4",
        "e3", "d5", "d2<", "d2", "2d4-", "d4", "c5", "b5", "2c2>", "d1+", "c2", "e2+",
        "d1", "e2", "c5<", "c5", "e4<", "Se4", "2b5-", "e4-", "a3-");

    assert_eu64(tak_perft(p, 0, slt, zd), 1);
    assert_eu64(tak_perft(p, 1, slt, zd), 152);
    assert_eu64(tak_perft(p, 2, slt, zd), 15356);
    assert_eu64(tak_perft(p, 3, slt, zd), 1961479);
}

int main() {
    tak_slt slt = tak_slt_fill();
    tak_zobrist_data zd = tak_zd_fill();

    test_5(&slt, &zd);
    test_6(&slt, &zd);
    test_stack(&slt, &zd);
    test_carry_limit(&slt, &zd);
    test_suicide(&slt, &zd);
    test_endgame(&slt, &zd);
    test_reserves(&slt, &zd);

    return 0;
}
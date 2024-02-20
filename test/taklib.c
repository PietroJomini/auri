#include <stdio.h>

#define TAKLIB_IMPLEMENTATION
#include "../lib/tak.h"

#define assert(cond)                                                              \
    ({                                                                            \
        const int __res = cond;                                                   \
        if (!__res)                                                               \
            fprintf(stderr, "[%s:%d] assertion failed: %s\n", __FILE__, __LINE__, \
                    #cond);                                                       \
    });

// count the amount of spreads derived from a given stack size
extern inline int count_spreads(const int h, const tak_slt* const slt) {
    return slt->indexes[h] - slt->indexes[h - 1];
}

// test slt
void test_slt() {
    const tak_slt slt = tak_slt_fill();

    // h=1
    //  1
    assert(count_spreads(1, &slt) == 1);
    assert(slt.spreads[slt.indexes[0]] == 0x1);

    // h=2
    //  2
    //  1 1
    assert(count_spreads(2, &slt) == 2);
    assert(slt.spreads[slt.indexes[1]] == 0x2);
    assert(slt.spreads[slt.indexes[1] + 1] == 0x11);

    // h=3
    //  3
    //  2 1
    //  1 2
    //  1 1 1
    assert(count_spreads(3, &slt) == 4);
    assert(slt.spreads[slt.indexes[2]] == 0x3);
    assert(slt.spreads[slt.indexes[2] + 1] == 0x12);
    assert(slt.spreads[slt.indexes[2] + 2] == 0x21);
    assert(slt.spreads[slt.indexes[2] + 3] == 0x111);

    // TODO: test other sizes
}

int main() {
    // sanity check
    assert(sizeof(tak_u8) == 1);
    assert(sizeof(tak_u32) == 4);
    assert(sizeof(tak_u64) == 8);

    // tests
    test_slt();  // spreads lookup table

    tak_slt slt = tak_slt_fill();
    printf("%ld\n", tak_perft(tak_newposition(6), 4, &slt));

    return 0;
}
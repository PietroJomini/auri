#pragma once

#include <stdio.h>
#include <sys/time.h>

#define BENCH_S()                            \
    {                                        \
        struct timeval before, after, epoch; \
        gettimeofday(&before, NULL);

#define BENCH_E()                                                      \
        gettimeofday(&after, NULL);                                        \
        timersub(&after, &before, &epoch);                                 \
        printf("[BENCH] %lf\n", epoch.tv_sec + epoch.tv_usec / 1000000.0); \
    }

#define BENCH(f) BENCH_S() f BENCH_E()
#define _GNU_SOURCE

#include "tei.h"

#include "notation.h"
#include "tak.h"

void tei_loop() {
    int argc, ccalled;
    char line[TEI_LINE_BUFSIZE], *argv[TEI_MAX_ARGC], *token, *saveptr;
    tei_data data = {.p = new_position(TEI_DEFAULT_SIZE), .slt = slt_fill(), .status = 1};
    tei_command commands[] = {
        {"quit", &_tei_quit},
        {"position", &_tei_position},
        {"print", &_tei_print},
        {"perft", &_tei_perft},
    };

    do {
        // read line from stdin
        if (fgets(line, TEI_LINE_BUFSIZE, stdin) == NULL) {
            if (feof(stdin)) exit(EXIT_SUCCESS);  // OEF
            else exit(EXIT_FAILURE);              // not eof, but stdin broke
        }

        // remove \n
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

        // split arguments
        argc = 0;
        for (token = strtok_r(line, " ", &saveptr); token != NULL;
             token = strtok_r(NULL, " ", &saveptr)) {
            argv[argc++] = token;
        }

        // call command
        if (argc == 0) continue;

        ccalled = 0;
        for (size_t i = 0; i < sizeof(commands) / sizeof(tei_command); i++)
            if (strcmp(argv[0], commands[i].command) == 0) {
                commands[i].action(&data, argc, argv);
                ccalled = 1;
            }

        if (ccalled == 0) {
        }  // TODO: print help

    } while (data.status);

    return;
}

void _tei_quit(tei_data *data, __attribute__((unused)) int argc,
               __attribute__((unused)) char **argv) {
    data->status = 0;
}

void _tei_position(tei_data *data, int argc, char **argv) {
    if (argc < 2) return;  // TODO: make a generalized logging system
    if (strcmp(argv[1], "tps") == 0 && argc >= 5) {
        char tps[TPS_MAX_LENGTH];
        snprintf(tps, TPS_MAX_LENGTH, "%s %s %s", argv[2], argv[3], argv[4]);
        data->p = tps2position(tps, TPS_STD);
    }
    if (strcmp(argv[1], "startpos") == 0) {
        int size = (argc >= 3) ? atoi(argv[2]) : TEI_DEFAULT_SIZE;
        if (size >= 3 && size <= 8) data->p = new_position(size);
    }

    // TODO: parse additional moves here
}

void _tei_print(tei_data *data, __attribute__((unused)) int argc,
                __attribute__((unused)) char **argv) {
    // tps
    char tps[TPS_MAX_LENGTH];
    position2tps(tps, data->p, TPS_STD);
    printf("Position %s\n", tps);

    // top border
    if (data->p.size > 3) {
        printf("  ┌─colors");
        for (int i = 0; i < data->p.size * 2 - 6; i++) printf("─");
        printf("┬─mods");
        for (int i = 0; i < data->p.size * 2 - 4; i++) printf("─");
        printf("┬─heights");
        for (int i = 0; i < data->p.size * 2 - 7; i++) printf("─");
        printf("┐ ┌─stacks");
        for (int i = 0; i < data->p.size * 9 - 8; i++) printf("─");
        printf("┐");
    } else {
        // shorter titles on 3x3
        printf("  ┌─col.");
        for (int i = 0; i < data->p.size * 2 - 4; i++) printf("─");
        printf("┬─mods");
        for (int i = 0; i < data->p.size * 2 - 4; i++) printf("─");
        printf("┬─heig.");
        for (int i = 0; i < data->p.size * 2 - 5; i++) printf("─");
        printf("┐ ┌─stacks");
        for (int i = 0; i < data->p.size * 9 - 8; i++) printf("─");
        printf("┐");
    }

    for (int y = 0; y < data->p.size; y++) {
        printf("\n%d ┤ ", y + 1);

        // color
        for (int x = 0; x < data->p.size; x++) {
            uint64_t i = 1ull << (y * data->p.size + x);
            char c = i & data->p.cbb[0] ? '0' : i & data->p.cbb[1] ? '1' : '-';
            printf("%c ", c);
        }
        printf("│ ");

        // wall and caps
        for (int x = 0; x < data->p.size; x++) {
            uint64_t i = 1ull << (y * data->p.size + x);
            char c = i & data->p.walls ? 'w' : i & data->p.caps ? 'c' : '-';
            printf("%c ", c);
        }
        printf("│");

        // heights
        // will be ugly with heights > 15, but well, i mean...
        for (int x = 0; x < data->p.size; x++) {
            int i = y * data->p.size + x;
            printf("%2x", data->p.heights[i]);
        }
        printf(" │ ");

        // stacks
        // will be ugly with stacks > ff, but well, i mean...
        for (int x = 0; x < data->p.size; x++) {
            int i = y * data->p.size + x;
            printf("│ %2d: %2lx ", i, data->p.stacks[i]);
        }
        printf("│");
    }

    // bottom border
    printf("\n  └─");
    for (int i = 0; i < data->p.size; i++) printf("┬─");
    printf("┴─");
    for (int i = 0; i < data->p.size; i++) printf("┬─");
    printf("┴─");
    for (int i = 0; i < data->p.size; i++) printf("┬─");
    printf("┘ └");
    for (int i = 0; i < data->p.size * 9 - 1; i++) printf("─");
    printf("┘\n  ");

    // letters
    for (int q = 0; q < 3; q++) {
        printf("  ");
        for (int i = 0; i < data->p.size; i++) printf("%c ", 'a' + i);
    }

    printf(data->p.size > 3 ? "    stp=[%d] mc=[%d] F=[%d %d] C=[%d %d]\n"
                            : "    stp=[%d] mc=[%d]\n                              F=[%d "
                              "%d] C=[%d %d]\n",
           data->p.stp, data->p.mc, data->p.reserves[0][0], data->p.reserves[1][0],
           data->p.reserves[0][1], data->p.reserves[1][1]);
}

void _tei_perft(tei_data *data, int argc, char **argv) {
    int depth = TEI_DEFAULT_DEPTH;
    if (argc >= 2) {
        depth = atoi(argv[1]);
        depth = (depth >= 0) ? depth : TEI_DEFAULT_DEPTH;
    }

    printf("%ld\n", perft(data->p, depth, &data->slt));
}
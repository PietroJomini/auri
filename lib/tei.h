/*
commands
- [x] help
- [x] quit
- [ ] tei: like `uci`. TODO: send options values?
- [x] print: pretty print the position
- [x] tps <tps>: load a tps string
- [x] new <size>: load an empty position of given size
- [x] move <...moves>: apply moves to the position
- [x] perft <depth>
- [x] perftd <depth>
- [ ] load: load position from a pre-loaded list of examples

settings
- [ ] opening (swap/no-swap)

cli options
- [ ] debug
*/

#ifndef TAKLIB_INCLUDE_TEI
#define TAKLIB_INCLUDE_TEI

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TAKLIB_IMPLEMENTATION
#undef TAKLIB_IMPLEMENTATION
#include "ptn.h"
#include "tak.h"
#include "ui.h"
#define TAKLIB_IMPLEMENTATION
#else
#include "ptn.h"
#include "tak.h"
#include "ui.h"
#endif

// to avoid using getline or a dinamic approach to fgets and srrtok
#define TEI_LINE_BUFSIZE 2048
#define TEI_MAX_ARGC 1024

// defaults
#define TEI_DEFAULT_SIZE 6

// tei-command communication statuses
typedef enum { TEI_OK = 1, TEI_QUIT = 2 } tei_status;

// payload passed to and from commands
typedef struct {
    tei_status status;
    tak_position pos;
    tak_slt slt;
    tak_zobrist_data zd;
} tei_payload;

// defines a single callable command
typedef struct {
    void (*action)(tei_payload *, int, char **);
    char *name;
} tei_command;

// command set
// used to make the global command utility
typedef struct {
    int size;      // amount of registered commands
    int capacity;  // allocated space in *commands
    tei_command *commands;
} tei_commandset;

// NOLINTNEXTLINE(misc-definitions-in-headers)
tei_commandset _global_set = {.size = 0, .capacity = 0};

// macro to define a new command
// i can'tdecide if this is ugly of not, but it's quite useful, so yeah
#define TEI_COMMAND(NAME, BODY)                                           \
    void _tei_##NAME(tei_payload *pl, int argc, char **argv) BODY;        \
    ({                                                                    \
        if (_global_set.capacity == 0) {                                  \
            _global_set.commands = malloc(sizeof(tei_command) * 10);      \
            _global_set.capacity = 10;                                    \
        } else if (_global_set.size >= _global_set.capacity) {            \
            _global_set.commands =                                        \
                realloc(_global_set.commands, _global_set.capacity + 10); \
            _global_set.capacity += 10;                                   \
        }                                                                 \
        _global_set.commands[_global_set.size++] =                        \
            (tei_command){.name = #NAME, .action = &_tei_##NAME};         \
    });

void tei_loop();
void tei();

#endif  // TAKLIB_INCLUDE_TEI

// #define TAKLIB_IMPLEMENTATION
#ifdef TAKLIB_IMPLEMENTATION

void tei_loop() {
    char line[TEI_LINE_BUFSIZE], *argv[TEI_MAX_ARGC], *token, *saveptr;
    tei_payload pl = {.status = TEI_OK,
                      .pos = tak_newposition(TEI_DEFAULT_SIZE),
                      .slt = tak_slt_fill(),
                      .zd = tak_zd_fill()};

    while (pl.status != TEI_QUIT) {
        // read line from stdin
        if (fgets(line, TEI_LINE_BUFSIZE, stdin) == NULL) {
            if (_global_set.commands) free(_global_set.commands);
            if (feof(stdin)) exit(EXIT_SUCCESS);  // OEF
            else exit(EXIT_FAILURE);              // not eof, but stdin broke (eg. ctr+c)
        }

        // remove \n
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';

        // split arguments
        int argc = 0;
        for (token = strtok_r(line, " ", &saveptr); token != NULL;
             token = strtok_r(NULL, " ", &saveptr)) {
            argv[argc++] = token;
        }

        // call commands
        if (argc == 0) continue;

        int called = 0;
        for (int i = 0; i < _global_set.size && !called; i++) {
            if (strcmp(argv[0], _global_set.commands[i].name) == 0) {
                _global_set.commands[i].action(&pl, argc - 1, argv + 1);
                called = 1;  // this is upper ugly and can be solved by a goto to a label
                             // after the "command not recognised"... what's worst?
            }
        }

        if (!called) {
            // TODO: cooler prints and logs
            printf("command \"%s\" not recognised. see `help` for a list of commands.\n",
                   argv[0]);
        }
    }
}

void tei() {
    // quit
    TEI_COMMAND(quit, { pl->status = TEI_QUIT; })

    // help
    TEI_COMMAND(help, {
        printf("available commands [%d/%d]\n", _global_set.size, _global_set.capacity);
        for (int i = 0; i < _global_set.size; i++)
            printf("- %s\n", _global_set.commands[i].name);
    })

    // print
    TEI_COMMAND(print, { tak_print(pl->pos); })

    // new
    TEI_COMMAND(new, {
        if (argc != 1) {
            printf("wrong number of arguments: 1 required\n");
            return;
        }

        int size = atoi(argv[0]);
        if (size < 3 || size > 8)
            printf("size %d not supported: range [3, 9] required\n", size);
        else pl->pos = tak_newposition(size);
    })

    // tps
    TEI_COMMAND(tps, {
        if (argc != 3) {
            printf("wrong number of arguments: 3 required\n");
            return;
        }

        // compose tps chunks into single string
        char tps[TPS_MAX_LENGTH];
        snprintf(tps, TPS_MAX_LENGTH, "%s %s %s", argv[0], argv[1], argv[2]);

        // load the new position
        pl->pos = tps_parse(tps, TPS_STD, &pl->zd);
    })

    // move
    TEI_COMMAND(move, {
        if (argv <= 0) {
            printf("wrong number of arguments: at least 1 required\n");
            return;
        }

        pl->pos = ptn_apply(pl->pos, argv, argc, PTN_STD, &pl->zd);
    })

    // perft
    TEI_COMMAND(perft, {
        if (argc != 1) {
            printf("wrong number of arguments: 1 required\n");
            return;
        }

        int depth = atoi(argv[0]);
        printf("%lu\n", tak_perft(pl->pos, depth, &pl->slt, &pl->zd));
    })

    // perftd
    TEI_COMMAND(perftd, {
        if (argc != 1) {
            printf("wrong number of arguments: 1 required\n");
            return;
        }

        int depth = atoi(argv[0]);
        tak_u64 nodes = 0;   // dunno why, but putting the two declarations in one lines
        tak_u64 nnodes = 0;  // breaks the macro (c wtf n. too much at this point lol)
        tak_move buffer[TAK_MAX_MOVES];
        int n = tak_search(buffer, &pl->pos, &pl->slt);
        char ptn[PTN_MAX_MOVE_LENGTH];

        for (int i = 0; i < n; i++) {
            nnodes = tak_perft(tak_do(pl->pos, buffer[i], &pl->zd), depth - 1, &pl->slt,
                               &pl->zd);
            nodes += nnodes;

            ptn_encode(ptn, buffer[i], pl->pos.size, PTN_STD);
            printf("\n%d. %s: %ld", i, ptn, nnodes);
        }

        printf("\n\nnodes searched: %ld\n", nodes);
    })

    // run the main loop
    tei_loop();
}

#endif  // TAKLIB_IMPLEMENTATION
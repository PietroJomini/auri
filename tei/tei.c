/*
commands
- [x] help
- [x] quit
- [ ] tei: like `uci`. TODO: send options values?
- [x] print <style>: print the position in a way determined by <style>
    - style=tps  print the tps of the position
    - style=pretty   (DEFAULT) pretty print all the position
- [x] new <size>: load an empty position of given size
- [x] tps <tps>: load a tps string
- [x] move <...moves>: apply moves to the position
- [x] perft <depth>
- [x] perftd <depth>
- [ ] load: load position from a pre-loaded list of examples
- [x] random [OPTIONS]: generate random position(s) starting from the set position
    - -m <n>: minimum depth
    - -M <n>: max depth
    - -n <n>: amount

cli options
- [ ] after `--` handle as input
*/

#include "tei.h"

#include <stdlib.h>

#define TAKLIB_IMPLEMENTATION
#include "../lib/ptn.h"
#include "../lib/tak.h"
#include "../lib/ui.h"

// default log level
FILE *tei_data_stream;
FILE *tei_log_stream;

void tei_loop(int n, tei_command *commands) {
    char line[TEI_LINE_BUFSIZE], *argv[TEI_MAX_ARGC], *token, *saveptr;

    tei_status status = TEI_OK;
    tei_payload pl = {.position = tak_newposition(TEI_DEFAULT_SIZE),
                      .slt = tak_slt_fill(),
                      .zd = tak_zd_fill()};

    while (status != TEI_QUIT) {
        // read line from stdin
        if (fgets(line, TEI_LINE_BUFSIZE, stdin) == NULL) {
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

        status = TEI_UNREC;
        for (int i = 0; i < n && status == TEI_UNREC; i++) {
            if (strcmp(argv[0], commands[i].name) == 0)
                status = commands[i].action(&pl, argc - 1, argv + 1, argc, argv);
        }

        // resolve non-quit statuses
        switch (status) {
            // unrecognised command
            case TEI_UNREC: tei_logw("command \"%s\" not recognised", argv[0]); break;

            // print command list
            case TEI_HELP:
                tei_print("available commands [%d]", n);
                for (int i = 0; i < n; i++) tei_print("- %s", commands[i].name);
                break;

            // wrong argument count
            case TEI_W_ARGC: tei_logw("wrong argument count"); break;

            default: break;
        }
    }
}

int tei_argce_check(tei_argce argce, int argc) {
    switch (argce.type) {
        case TEI_ARGC_ANY: return 0;
        case TEI_ARGC_EXACT:
            if (argce.exact == argc) return 0;
            else break;
        case TEI_ARGC_MIN:
            if (argce.min <= argc) return 0;
            else break;
        case TEI_ARGC_RANGE:
            if (argce.min <= argc && argc <= argce.max) return 0;
            else break;
    }

    return 1;
}

TEI_ACTION(help, ANY, { return TEI_HELP; })
TEI_ACTION(quit, ANY, { return TEI_QUIT; })

// print
TEI_ACTION(
    print, RANGE,
    {
        // TODO: remove this and replace wih opening a tps.ninja url or image
        //       `tak_print` can be replaced by a debug whole struct dump
        if (argc == 0) tak_print(pl->position);
        else {
            if (strcmp(argv[0], "pretty") == 0) tak_print(pl->position);
            else if (strcmp(argv[0], "tps") == 0) {
                char tps[TPS_MAX_LENGTH];
                tps_encode(tps, pl->position, TPS_STD);
                tei_print("%s", tps);
            } else tei_logw("option \"%s\" not recognised", argv[0]);
        }
    },
    .max = 1)

// new
TEI_ACTION(
    new, EXACT,
    {
        int size = atoi(argv[0]);
        if (size < 3 || size > 8) tei_logw("size %d not supported", size);
        else pl->position = tak_newposition(size);
    },
    1)

// tps
TEI_ACTION(
    tps, EXACT,
    {
        // compose tps chunks into single string
        char tps[TPS_MAX_LENGTH];
        snprintf(tps, TPS_MAX_LENGTH, "%s %s %s", argv[0], argv[1], argv[2]);

        // load the new position
        pl->position = tps_parse(tps, TPS_STD, &pl->zd);
    },
    3)

// move
// TODO: if i have 0 argc it just does nothing, how strict do i want to be?
TEI_ACTION(
    move, MIN, { pl->position = ptn_apply(pl->position, argv, argc, PTN_STD, &pl->zd); },
    1)

// perft
TEI_ACTION(
    perft, EXACT,
    { tei_print("%lu", tak_perft(pl->position, atoi(argv[0]), &pl->slt, &pl->zd)); }, 1)

// perftd
TEI_ACTION(
    perftd, EXACT,
    {
        int depth = atoi(argv[0]);
        tak_u64 nodes = 0;   // dunno why, but putting the two declarations in one line
        tak_u64 nnodes = 0;  // breaks the macro (c wtf n. too much at this point lol)
        tak_move buffer[TAK_MAX_MOVES];
        int n = tak_search(buffer, &pl->position, &pl->slt);
        char ptn[PTN_MAX_MOVE_LENGTH];

        for (int i = 0; i < n; i++) {
            nnodes = tak_perft(tak_do(pl->position, buffer[i], &pl->zd), depth - 1,
                               &pl->slt, &pl->zd);
            nodes += nnodes;

            ptn_encode(ptn, buffer[i], pl->position.size, PTN_STD);
            tei_print("%d. %s: %ld", i, ptn, nnodes);
        }

        tei_print("%ld", nodes);
    },
    1)

TEI_ACTION(random, ANY, {
    int c;
    int m = TEI_RANDOM_DEFAULT_MIN;
    int M = TEI_RANDOM_DEFAULT_MAX;
    int n = TEI_RANDOM_DEFAULT_N;

    // parse options
    while ((c = getopt(argc_o, argv_o, "n:")) != -1) {
        switch (c) {
            case 'm': m = atoi(optarg); break;
            case 'M': M = atoi(optarg); break;
            case 'n': n = atoi(optarg); break;
            case '?':
                return TEI_OK;  // TODO: getopt has a default warning mesasge when passing
                                // an unknown options. bypass that when i refactor the
                                // output style?
        }
    }

    // should i throw on this kind of errors?
    // TODO: print a warning if not in machine mode
    if (m > M) m = M;
    if (n < 1) n = 1;

    srand(time(NULL));
    char tps[TPS_MAX_LENGTH];
    tak_move buffer[TAK_MAX_MOVES];

    for (int i = 0; i < n; i++) {
        tak_position p = pl->position;
        int depth = RANDINT(m, M);
        int ended = 0;

        // get deeper into the randomification
        for (int i = 0; i < depth && !ended; i++) {
            int nm = tak_search(buffer, &p, &pl->slt);
            tak_position np = tak_do(p, buffer[RANDINT(0, nm - 1)], &pl->zd);

            // check if the game is ended
            if (tak_check_ending(&np, 0).ended) ended = 1;
            else p = np;
        }

        tps_encode(tps, p, TPS_STD);
        tei_print("%s", tps);
    }
})

int main(int argc, char **argv) {
    // set out streams
    tei_data_stream = stdout;
    tei_log_stream = stderr;

    tei_loop(9, (tei_command[]){
                    TEI_CMD(help),
                    TEI_CMD(quit),
                    TEI_CMD(print),
                    TEI_CMD(new),
                    TEI_CMD(tps),
                    TEI_CMD(move),
                    TEI_CMD(perft),
                    TEI_CMD(perftd),
                    TEI_CMD(random),
                });

    return EXIT_SUCCESS;
}
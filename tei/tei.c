/*
commands
- [x] help
- [x] quit
- [ ] tei: like `uci`. TODO: send options values?
- [x] print <style>: print the position in a way determined by <style>
    - style=tps  print the tps of the position
    - style=pretty   (DEFAULT) pretty print all the position
- [x] ninja: print the url pointing to the current position in tps.ninja
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

// for strtok_r
#define _GNU_SOURCE

#include "tei.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAKLIB_IMPLEMENTATION
#include "../lib/ptn.h"
#include "../lib/tak.h"
#include "../lib/ui.h"

// default log level
FILE *tei_data_stream;
FILE *tei_log_stream;

void tei_loop(int n, tei_command *commands, int na, tei_alias *aliases) {
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

        // try to resolve aliases first
        // would be more efficent to try doing it after having looket through original
        // commands, but this way i won't have to extrapolate the invoking and argce login
        // into a function. with just a couple of aliases and commands it will be ok, but
        // in the future i may have to fix this
        int alias_solved = 0;
        for (int i = 0; i < na && alias_solved == 0; i++) {
            if (strcmp(argv[0], aliases[i].alias) == 0) {
                argv[0] = aliases[i].target;
                alias_solved = 1;
            }
        }

        status = TEI_UNREC;
        for (int i = 0; i < n && status == TEI_UNREC; i++) {
            if (strcmp(argv[0], commands[i].name) == 0) {
                // check argce count
                int argce_status = 0, argceff = argc - 1;
                if (commands[i].argce.type == TEI_ARGC_ANY) {
                    argce_status = 1;
                } else if (commands[i].argce.type == TEI_ARGC_EXACT) {
                    if (commands[i].argce.exact == argceff) argce_status = 1;
                } else if (commands[i].argce.type == TEI_ARGC_MIN) {
                    if (commands[i].argce.min <= argceff) argce_status = 1;
                } else if (commands[i].argce.type == TEI_ARGC_RANGE) {
                    if (commands[i].argce.min <= argceff &&
                        argceff <= commands[i].argce.max)
                        argce_status = 1;
                }

                // if we have the right `argc`, invoke the command
                if (argce_status == 0) status = TEI_W_ARGC;
                else status = commands[i].action(&pl, argceff, argv + 1, argc, argv);
            }
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

TEI_ACTION(help, { return TEI_HELP; })
TEI_ACTION(quit, { return TEI_QUIT; })

// print
TEI_ACTION(print, {
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
})

// print the ptn.ninja url for the fiven position
// I could create a local image with tpsninja, but i guess it would
// have downsides both in complexity and customization
TEI_ACTION(ninja, {
    // i'll break the cool log stff a bit here...
    if (tei_data_stream == NULL) return TEI_OK;

    char tps[TPS_MAX_LENGTH];
    int n = tps_encode(tps, pl->position, TPS_STD);
    fprintf(tei_data_stream, "%s/[tps%%20\"", TEI_NINJA_BASE_URL);

    // really crude urlencode
    // can't do this with a lexicon, as it can't handle strings :(
    // with a modern browser it won't really matter, but this makes it compatible with
    // things like `xdg-open` or terminal url-clicking, making it way more flexible
    for (int i = 0; i < n; i++)
        if (tps[i] == ' ') fprintf(tei_data_stream, "%%20");
        else fprintf(tei_data_stream, "%c", tps[i]);
    tei_print("\"]");
})

// new
TEI_ACTION(new, {
    int size = atoi(argv[0]);
    if (size < 3 || size > 8) tei_logw("size %d not supported", size);
    else pl->position = tak_newposition(size);
})

// tps
TEI_ACTION(tps, {
    // compose tps chunks into single string
    char tps[TPS_MAX_LENGTH];
    snprintf(tps, TPS_MAX_LENGTH, "%s %s %s", argv[0], argv[1], argv[2]);

    // load the new position
    pl->position = tps_parse(tps, TPS_STD, &pl->zd);
})

// move
// TODO: if i have 0 argc it just does nothing, how strict do i want to be?
// TODO: should i check legality? or at least playability?
TEI_ACTION(move,
           { pl->position = ptn_apply(pl->position, argv, argc, PTN_STD, &pl->zd); })

// perft
TEI_ACTION(perft, {
    tei_print("%lu", tak_perft(pl->position, atoi(argv[0]), &pl->slt, &pl->zd));
})

// perftd
TEI_ACTION(perftd, {
    int depth = atoi(argv[0]);
    tak_u64 nodes = 0;   // dunno why, but putting the two declarations in one line
    tak_u64 nnodes = 0;  // breaks the macro (c wtf n. too much at this point lol)
    tak_move buffer[TAK_MAX_MOVES];
    int n = tak_search(buffer, &pl->position, &pl->slt);
    char ptn[PTN_MAX_MOVE_LENGTH];

    for (int i = 0; i < n; i++) {
        nnodes = tak_perft(tak_do(pl->position, buffer[i], &pl->zd), depth - 1, &pl->slt,
                           &pl->zd);
        nodes += nnodes;

        ptn_encode(ptn, buffer[i], pl->position.size, PTN_STD);
        tei_print("%d. %s: %ld", i, ptn, nnodes);
    }

    tei_print("%ld", nodes);
})

TEI_ACTION(random, {
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

    // commands
    tei_command commands[] = {
        TEI_CMD(help, ANY),
        TEI_CMD(quit, ANY),
        TEI_CMD(print, RANGE, .max = 1),
        TEI_CMD(ninja, ANY),
        TEI_CMD(new, EXACT, .exact = 1),
        TEI_CMD(tps, EXACT, .exact = 3),
        TEI_CMD(move, MIN, .min = 1),
        TEI_CMD(perft, EXACT, .exact = 1),
        TEI_CMD(perftd, EXACT, .exact = 1),
        TEI_CMD(random, ANY),
    };

    // aliases
    tei_alias aliases[] = {
        {"p", "print"},
        {"q", "quit"},
    };

    tei_loop(10, commands, 2, aliases);
    return EXIT_SUCCESS;
}
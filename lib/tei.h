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

// tei-command communication statuses
typedef enum { TEI_OK = 1, TEI_QUIT = 2 } tei_status;

// payload passed to and from commands
typedef struct {
    tei_status status;
} tei_payload;

// defines a single callable command
typedef struct {
    void (*action)(tei_payload *, char **);
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
    void _tei_##NAME(tei_payload *pl, char **argv) BODY;                  \
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

#ifdef TAKLIB_IMPLEMENTATION

void tei_loop() {
    char line[TEI_LINE_BUFSIZE], *argv[TEI_MAX_ARGC], *token, *saveptr;
    tei_payload pl = {.status = TEI_OK};

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
                _global_set.commands[i].action(&pl, argv);
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
    // define commands
    TEI_COMMAND(quit, { pl->status = TEI_QUIT; })
    TEI_COMMAND(help, {
        printf("available commands [%d/%d]\n", _global_set.size, _global_set.capacity);
        for (int i = 0; i < _global_set.size; i++)
            printf("- %s\n", _global_set.commands[i].name);
    })

    // run the main loop
    tei_loop();
}

#endif  // TAKLIB_IMPLEMENTATION
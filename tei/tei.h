#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../lib/ptn.h"
#include "../lib/tak.h"
#include "../lib/ui.h"

// to avoid using getline or a dinamic approach to fgets and strtok
#define TEI_LINE_BUFSIZE 2048
#define TEI_MAX_ARGC 1024

// defaults
#define TEI_DEFAULT_SIZE 6
#define TEI_RANDOM_DEFAULT_MIN 1
#define TEI_RANDOM_DEFAULT_MAX 100
#define TEI_RANDOM_DEFAULT_N 1

// helpers
#define RANDINT(A, B) (A + rand() % (B - A + 1))

// tei-command communication statuses
typedef enum {
    TEI_OK,    // the loop should continue as normal
    TEI_QUIT,  // the loop should quit

    // warnings
    TEI_HELP,  // print the commands list
} tei_status;

// payload passed to and from commands
typedef struct {
    tak_position position;
    tak_zobrist_data zd;
    tak_slt slt;
} tei_payload;

// tei action params
#define TEI_ACTION_PARAMS                                       \
    tei_payload *pl,  /*data payload*/                          \
        int argc,     /*command-specific argc*/                 \
        char **argv,  /*command-specific argv*/                 \
        int argc_o,   /*original argc (included the arg name)*/ \
        char **argv_o /*original argv (included the arg name)*/

// defines a single callable command
typedef struct {
    // passing also the original argc and argv (the second set) allows to use `getopt`
    tei_status (*action)(TEI_ACTION_PARAMS);
    char *name;
} tei_command;

// create a tei action
#define TEI_ACTION(NAME, BODY)                  \
    tei_status _tei_##NAME(TEI_ACTION_PARAMS) { \
        BODY;                                   \
        return TEI_OK;                          \
    }

// create a tei command
#define TEI_CMD(NAME) \
    { .action = &_tei_##NAME, .name = #NAME }

// main tei IO loop
void tei_loop(int n, tei_command *commands);
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
    TEI_HELP,    // print the commands list
    TEI_UNREC,   // unrecognised command
    TEI_W_ARGC,  // wrong argc
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

// expected amount of arguments flags
typedef enum {
    TEI_ARGC_ANY,    // don't check
    TEI_ARGC_EXACT,  // expec exact amount
    TEI_ARGC_MIN,    // expect argc to be more than
    TEI_ARGC_RANGE,  // expect argc in range
} tei_argce_t;

// expected amount of arguments data
typedef struct {
    tei_argce_t type;
    union {
        int exact;  // TEI_ARGC_EXACT
        struct {    // TEI_ARGC_RANGE
            int min;
            int max;
        };
    };
} tei_argce;

// check tei_argcs
int tei_argce_check(tei_argce argce, int argc);

// create a tei action
#define TEI_ACTION(NAME, ARGCE_T, BODY, ...)                                    \
    tei_status _tei_##NAME(TEI_ACTION_PARAMS) {                                 \
        /*check argc*/                                                          \
        tei_argce argce = (tei_argce){.type = TEI_ARGC_##ARGCE_T, __VA_ARGS__}; \
        if (tei_argce_check(argce, argc)) return TEI_W_ARGC;                    \
                                                                                \
        BODY;                                                                   \
        return TEI_OK; /*if BODY doesn't return, default to TEI_OK*/            \
    }

// create a tei command
#define TEI_CMD(NAME) \
    { .action = &_tei_##NAME, .name = #NAME }

// main tei IO loop
void tei_loop(int n, tei_command *commands);

// streams
extern FILE *tei_data_stream;
extern FILE *tei_log_stream;

// print data to `data` stream
#define tei_print(...)                             \
    ({                                             \
        if (tei_data_stream != NULL) {             \
            fprintf(tei_data_stream, __VA_ARGS__); \
            fprintf(tei_data_stream, "\n");        \
        }                                          \
    })

// print to log
#define tei_log(LEVEL, ...)                                                       \
    ({                                                                            \
        if (tei_log_stream != NULL) {                                             \
            time_t t = time(NULL);                                                \
            struct tm tm = *localtime(&t);                                        \
            fprintf(tei_log_stream, "%02d:%02d:%02d %s: ", tm.tm_hour, tm.tm_min, \
                    tm.tm_sec, #LEVEL);                                           \
            fprintf(tei_log_stream, __VA_ARGS__);                                 \
            fprintf(tei_log_stream, "\n");                                        \
        }                                                                         \
    })

// common log levels
// TODO: add a way to ignore certain levels and add a bunch of info (or trace) logs
#define tei_logi(...) tei_log(INFO, __VA_ARGS__)
#define tei_logw(...) tei_log(WARNING, __VA_ARGS__)
#define tei_loge(...) tei_log(ERROR, __VA_ARGS__)
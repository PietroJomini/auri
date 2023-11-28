#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tak.h"

// to avoid using getline or a dinamic approach to fgets and srrtok
#define TEI_LINE_BUFSIZE 2048
#define TEI_MAX_ARGC 10

// default stuff
#define TEI_DEFAULT_SIZE 6
#define TEI_DEFAULT_DEPTH 3

typedef struct {
    position p;
    slides_lt slt;
    int status;
} tei_data;

typedef struct {
    char *command;
    void (*action)(tei_data *, int, char **);
} tei_command;

// main read-parse-print loop
void tei_loop();

// commands
void _tei_quit(tei_data *, int, char **);
void _tei_position(tei_data *, int, char **);
void _tei_print(tei_data *, int, char **);
void _tei_perft(tei_data *, int, char **);
#pragma once

#include <stdint.h>

typedef struct parse {
    char *root;
    uint16_t port;
} parse;

parse *parse_arg(int argc, char **argv);
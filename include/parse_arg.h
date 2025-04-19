#pragma once

#include <stdint.h>

typedef struct parse {
    const char *root;
    const uint16_t *port;
} parse;

void print_helper();
int parse_dirname(char *path, parse *parse_s);
int parse_port(char *port, parse *parse_s);
parse *parse_arg(int argc, char **argv);
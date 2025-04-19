#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/parse_arg.h"
#include "../include/utils.h"

void print_helper() {
    printf("Usage: my-server [PORT] [ROOT_DIR]\nOpen a web server on PORT with ROOT_DIR as root.\n\nExamples:\n   ./my-server 8080 ./root/\n");
}

int parse_dirname(char *path, parse *parse_s) {
    if (access(path, F_OK | R_OK)) {
        printf("my-server: %s is not a dir or not readable\n", path);
        return 1;
    }

    parse_s->root = (const char *) path;

    return 0;
}

int parse_port(char *port, parse *parse_s) {
    for (int i = 0; port[i] != '\0'; i++) {
        if (port[i] < '0' || port[i] > '9') {
            printf("my-server: %s is not a port number\nTry 'my-server --help' for more information.\n", port);
            return 1;
        }
    }

    parse_s->port = (const uint16_t) char_to_port(port);

    return 0;
}

parse *parse_arg(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (strncmp(argv[i], "--help", 6) == 0) {
            print_helper();
            return NULL;
        }
    }

    if (argc < 3) {
        printf("my-server: missing arg\nTry 'my-server --help' for more information.\n");
        return NULL;
    }

    parse *parse_s = calloc(1, sizeof(parse));

    if (parse_port(argv[1], parse_s)) {
        free(parse_s);
        return NULL;
    }

    if (parse_dirname(argv[2], parse_s)) {
        free(parse_s);
        return NULL;
    }

    return parse_s;
}
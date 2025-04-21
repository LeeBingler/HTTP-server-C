#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "../../include/config.h"
#include "../../include/core/parse_arg.h"
#include "../../include/utils/utils.h"

static void print_helper() {
    printf(
        "Usage: my-server [OPTIONS]\n"
        "Open a web server on PORT with ROOT_DIR as root.\n\n"
        "Options:\n"
        "  -p, --port [PORT]       Specify the port number (default: 8080)\n"
        "  -r, --root [ROOT_DIR]   Set the root directory (default: ./)\n"
        "  --help                  Show this help message\n\n"
        "Examples:\n"
        "  ./my-server -p 8080 -r ./www/\n"
    );
}

static int parse_dirname(char *path, parse *parse_s) {
    if (access(path, F_OK | R_OK)) {
        fprintf(stderr, "my-server: '%s' is not a directory or is not readable.\n", path);
        fprintf(stderr, "Try 'my-server --help' for more information.\n");
        return 1;
    }

    size_t len = strlen(path);
    if (len > 1 && path[len - 1] == '/') {
        path[len - 1] = '\0';
    }

    parse_s->root = path;

    return 0;
}

static int parse_port(char *port, parse *parse_s) {
    for (int i = 0; port[i] != '\0'; i++) {
        if (port[i] < '0' || port[i] > '9') {
            fprintf(stderr, "my-server: '%s' is not a valid port number.\n", port);
            fprintf(stderr, "Try 'my-server --help' for more information.\n");
            return 1;
        }
    }

    parse_s->port = char_to_port(port);

    return 0;
}

parse *parse_arg(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_helper();
            return NULL;
        }
    }

    parse *parse_s = calloc(1, sizeof(parse));

    if (!parse_s) {
        perror("calloc parse_s");
        return NULL;
    }

    for (int i = 0; i < argc; i++) {
        if ((strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) && i + 1 < argc) {
            if (parse_port(argv[i + 1], parse_s)) {
                free(parse_s);
                return NULL;
            }

        } else if ((strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--root") == 0) && i + 1 < argc) {
            if (parse_dirname(argv[i + 1], parse_s)) {
                free(parse_s);
                return NULL;
            }
        }
    }

    // Set defaults if not specified
    if (!parse_s->port) parse_s->port = htons(DEFAULT_PORT);
    if (!parse_s->root) parse_s->root = DEFAULT_ROOT;


    return parse_s;
}
#include <unistd.h>
#include "include/http-server.h"

void print_helper() {
    printf("Usage: my-server [PORT] [ROOT_DIR]\nOpen a web server on PORT with ROOT_DIR as root.\n\nExamples:\n   ./my-server 8080 ./root/\n");
}

int parse_dirname(char *path) {
    if (access(path, F_OK | R_OK)) {
        printf("my-server: %s is not a dir or not readable\n", path);
        return 1;
    }

    return 0;
}

int parse_arg(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (strncmp(argv[i], "--help", 6) == 0) {
            print_helper();
            return 1;
        }
    }

    if (argc < 3) {
        printf("my-server: missing arg\nTry 'my-server --help' for more information.\n");
        return 1;
    }

    for (int i = 0; argv[1][i] != '\0'; i++) {
        if (argv[1][i] < '0' || argv[1][i] > '9') {
            printf("my-server: %s is not a port number\nTry 'my-server --help' for more information.\n", argv[1]);
            return 1;
        }
    }

    if (parse_dirname(argv[2]))
        return 1;

    return 0;
}
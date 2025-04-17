#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "include/http-server.h"

volatile sig_atomic_t running = 1;

void handle_sigint(int sig) {
    running = 0;
}

void print_helper() {
    printf("Usage: my-server [PORT] [ROOT_DIR]\nOpen a web server on PORT with ROOT_DIR as root.\n\nExamples:\n   ./my-server 8080 ./root/\n");
}

int parser_arg(int argc, char **argv) {
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
            printf("my-server: arg 2 is not a port number\nTry 'my-server --help' for more information.\n");
            return 1;
        }
    }

    return 0;
}

uint16_t to_port(char *port_nb) {
    char *output;
    long port = strtol(port_nb, &output, 10);

    return htons((uint16_t) port); // htons reverse hexa value of port number
}

int main(int argc, char **argv) {
    if (parser_arg(argc, argv))
        return 1;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    unsigned int port = to_port(argv[1]);

    if (server_fd == -1) {
        perror("server did not open");
        return errno;
    }
    int true = 1;
    if (setsockopt(server_fd,SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int)) == -1) {
        perror("setsockopt()");
        close(server_fd);
        return errno;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = port,
        .sin_addr.s_addr = INADDR_ANY // Listen on all port
    };

    if (bind(server_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind()");
        close(server_fd);
        return errno;
    }

    if (listen(server_fd, 10) == -1) {
        perror("listen()");
        close(server_fd);
        return errno;
    }

    signal(SIGINT, handle_sigint);

    struct sockaddr_in client_addr = { 0 };
    unsigned int addrlen = sizeof(client_addr);
    int client_fd = 0;

    while (running) {
        client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &addrlen);

        if (client_fd == -1) {
            perror("accept()");
            close(server_fd);
            return errno;
        }

        printf("Connection: %i\n", client_addr.sin_port);
        handle_client(client_fd);
    }

    close(server_fd);
    return 0;
}

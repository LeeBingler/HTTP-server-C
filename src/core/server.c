#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "../../include/config.h"
#include "../../include/http/handle_client.h"
#include "../../include/core/parse_arg.h"

static volatile sig_atomic_t running = 1;

void handle_sigint(int sig) {
    (void)sig;
    running = 0;
}

int setup_server_socket(uint16_t port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("server did not open");
        return -1;
    }

    // addr reusabble after crash
    int enable = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
        perror("setsockopt()");
        close(server_fd);
        return -1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = port,
        .sin_addr.s_addr = INADDR_ANY // Listen on all port
    };

    if (bind(server_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind()");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, CONNECTION_BACKLOG) == -1) {
        perror("listen()");
        close(server_fd);
        return -1;
    }

    return server_fd;
}

int main(int argc, char **argv) {
    parse *parse_s = parse_arg(argc, argv);
    if (!parse_s) return EXIT_FAILURE;

    int server_fd = setup_server_socket(parse_s->port);
    if (server_fd == -1) {
        free(parse_s);
        return EXIT_FAILURE;
    }

    signal(SIGINT, handle_sigint);
    printf("Server listen on : http://localhost:%i\n", ntohs(parse_s->port));

    struct sockaddr_in client_addr = { 0 };
    unsigned int addrlen = sizeof(client_addr);
    int client_fd = 0;

    while (running) {
        client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &addrlen);

        if (client_fd == -1) {
            if (errno == EINTR) continue; // Interrupted by SIGINT
            perror("accept()");
            break;
        }

        handle_client(client_fd, client_addr, parse_s->root);
    }

    close(server_fd);
    free(parse_s);
    printf("\nServer shut down gracefully.\n");
    return EXIT_SUCCESS;
}

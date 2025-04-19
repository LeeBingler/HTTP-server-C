#include <errno.h>
#include <signal.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "../include/http-server.h"
#include "../include/utils.h"
#include "../include/parse_arg.h"

volatile sig_atomic_t running = 1;

void handle_sigint(int sig) {
    running = 0;
}

int main(int argc, char **argv) {
    parse *parse_s = parse_arg(argc, argv);
    if (!parse_s) return 808;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
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
        .sin_port = parse_s->port,
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
    printf("Server listen on : http://localhost:%i\n", ntohs(parse_s->port));

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

        handle_client(client_fd, client_addr, parse_s->root);
    }

    close(server_fd);
    free(parse_s);
    return 0;
}

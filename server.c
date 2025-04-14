#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int parser(int argc, char **argv) {
    if (argc != 2) {
        printf("Error: Missing port number\n");
        return 1;
    }
    
    for (int i = 0; argv[1][i] != '\0'; i++) {
        if (argv[1][i] < '0' || argv[1][i] > '9') {
            printf("Error: arg 2 is not a port number\n");
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
    if (parser(argc, argv))
        return 1;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    unsigned int port = to_port(argv[1]);

    if (server_fd == -1) {
        perror("server did not open");
        return errno;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = port,
        .sin_addr.s_addr = INADDR_ANY // Listen on all port
    };

    if (bind(server_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind didn't work");
        close(server_fd);
        return errno;
    }
    
    if (listen(server_fd, 10) == -1) {
        perror("listen() didn't work");
        close(server_fd);
        return errno;
    }

    struct sockaddr_in client_addr;
    unsigned int addrlen = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &addrlen);
    
    if (client_fd == -1) {
        perror("accept()");
        close(server_fd);
        return 1;
    } 

    printf("Connection: %i\n", client_addr.sin_port);

    char buff[256] = { 0 };
    int len = recv(client_fd, buff, 256, 0);
    
    if (len == -1) {
        perror("recv()");
        close(server_fd);
        close(client_fd);
        return 1;

    } else if (len == 0) {
        printf("User disconnected");

    } else if (len > 0) {
        // client send data
        char mess[2] = "Hi";
        send(client_fd, mess, 2, MSG_DONTWAIT);
        printf("send\n");
    }

    printf("works\n");
    close(server_fd);
    close(client_fd);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int parser(int argc, char **argv) {
    if (argc != 2)
        return 1;
    
    
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
        .sin_addr.s_addr = 0
    };

    if (bind(server_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind didn't work");
        close(server_fd);
        return errno;
    }
    
    if (listen(server_fd, 10) == -1) {
        perror("bind didn't work");
        close(server_fd);
        return errno;
    }
    
    int client_fd = accept(server_fd, 0, 0); // &addr, (socklen_t *) sizeof(addr)
    
    if (client_fd == -1) {
        perror("accept fail");
        close(server_fd);
        return 1;
    }
    
    int size_buff = 256;
    char buff[size_buff] = { 0 };

    recv(server_fd, buff, size_buff, 0);
    
    printf("%s\n", buff);

    close(server_fd);
    close(client_fd);
    return 0;
}

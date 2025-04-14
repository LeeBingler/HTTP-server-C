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
    if (parser(argc, argv)) {
        return 2;
    }

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    unsigned int port = to_port(argv[1]);

    if (client_fd == -1) {
        perror("server did not open");
        return errno;
    }

    int dest_port;

    printf("Port to connect: ");
    scanf("%i", &dest_port);

    struct sockaddr_in dest_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(dest_port),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };

    if (connect(client_fd, (struct sockaddr *) &dest_addr, sizeof(dest_addr) == -1)) {
        perror("Connect did not work");
        close(server_fd);
        return errno;
    }

    char buff[4] = "Hi !";

    send(client_fd, buff, 4, 0);

    close(client_fd);
    return EXIT_SUCCESS;
}

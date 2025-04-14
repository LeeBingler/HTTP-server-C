#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void) {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_fd == -1) {
        perror("socket did not open");
        return errno;
    }

    int dest_port;

    printf("Port to connect: ");
    scanf("%i", &dest_port);

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(dest_port),
        .sin_addr.s_addr = inet_addr("127.0.0.1") //IP localhost
    };

    if (connect(client_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
        perror("Connect did not work");
        close(client_fd);
        return errno;
    }

    char *buff = "Hi !";

    send(client_fd, buff, 4, 0);

    close(client_fd);
    return EXIT_SUCCESS;
}

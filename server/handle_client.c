#include "../include/http-server.h"

#define BUFF_SIZE 1024

int isget_request(request_t *request) {
    if (strcmp(request->method, "GET") == 0) {
        return 1;
    }

    return 0;
}

int handle_client(int client_fd) {
    char buff[BUFF_SIZE] = { 0 };
    int byte_recv = recv(client_fd, buff, BUFF_SIZE, MSG_DONTWAIT);

    if (byte_recv == -1) {
        perror("recv()");
        close(client_fd);
        return errno;

    } else if (byte_recv == 0) {
        printf("Client disconnected\n");
        close (client_fd);
        return 0;
    }

    request_t *request = parse_request(buff);
    if (request == NULL)
        return errno;

    if (isget_request(request)) {
        char *mess_header = "HTTP/1.0 200 OK\n"; // Content-type + Date + connection + transfer-encoding
        send(client_fd, mess_header, strlen(mess_header), MSG_CONFIRM);
    } else {
        char *mess_header = "HTTP/1.0 501 Not Implemented\n";
        send(client_fd, mess_header, strlen(mess_header), MSG_CONFIRM);
    }

    printf("send\n");

    close(client_fd);
    free_request(request);
    return 0;
}

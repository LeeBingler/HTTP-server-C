#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>

#define BUFF_SIZE 1080

typedef struct {
    char *method;
    char *path;
    char *protocol;
    char *headers;
    char *data;
} header_t;

header_t *parse_request(char *request) {
    header_t *header = malloc(sizeof(header_t));
    char *cp_request = malloc(sizeof(char) * (BUFF_SIZE + 1));
    
    if(strcpy(cp_request, request) == NULL) {
        perror("strcpy() in parse_request");
        free(header);
        return NULL;
    }

    if (header == NULL) {
        perror("calloc() in parse_request");
        return header;
    }

    printf("cp_request:\n\n%s\n", cp_request);

    return header;
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

    header_t *header = parse_request(buff);
    if (header == NULL)
        return errno;

    char mess_header[17] = "HTTP/1.1 200 OK\n"; // Content-type + Date + connection + transfer-encoding
    send(client_fd, mess_header, 17, MSG_CONFIRM);
    printf("send\n");

    close(client_fd);
    free(header);
    return 0;
}

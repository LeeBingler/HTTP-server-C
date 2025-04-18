#include <stdio.h>

#include "include/http-server.h"
#include "include/http-headers.h"

#define BUFF_SIZE 1024

int get_request(request_t *request, int client_fd) {
    size_t nbytes = 0;
    char response[BUFF_SIZE];
    FILE *file = fopen(request->path, "rb");

    if (!file) {
        if (errno == ENOENT) {
            printf("Erreur 404\n");
            send(client_fd, "HTTP/1.1 404 Not Found\r\n", 25, 0);
            return 404;
        }

        if (errno == EACCES) {
            printf("Erreur 403\n");
            send(client_fd, "HTTP/1.1 403 Forbidden\r\n", 25, 0);
            return 403;
        }

        printf("errno %i : %s\n", errno, strerror(errno));
        return errno;
    }
    // Send response
    send(client_fd, "HTTP/1.1 200 OK\r\n", 18, 0);

    // Send headers
    send_date(client_fd);
    send_contentlenght(client_fd, file);
    send_contenttype(client_fd, request->path);
    send_connection(client_fd);

    send(client_fd, "\r\n", 2, 0);  // end of headers

    // Send file
    while ((nbytes = fread(response, sizeof(char), BUFF_SIZE, file)) > 0) {
        int offset = 0;
        int sent = 0;
        while ((sent = send(client_fd, response + offset, nbytes, 0)) > 0 || (sent == -1 && errno == EINTR) ) {
            if (sent > 0) {
                offset += sent;
                nbytes -= sent;
            }
        }
    }
    errno = 0;

    fclose(file);
    return 0;
}

int normalize_request_path(request_t *request, char *path_root) {
    int len_path = strlen(request->path);
    int len_root = strlen(path_root);

    // make index.html default if the path end by "/" ==> move it before get_request call
    if (request->path[len_path - 1] == '/') {
        char *str_index = "index.html";
        int len_index = strlen(str_index);
        char *new_path = calloc(len_path + len_index + 1, sizeof(char));

        if (!new_path) return -1;

        strncpy(new_path, request->path, len_path);
        strncat(new_path, str_index, len_index);
        free(request->path);

        request->path = new_path;
    }

    // add path_root to path
    len_path = strlen(request->path);
    char *new_path = calloc(len_path + len_root + 1, sizeof(char));
    if (!new_path) return -1;

    memcpy(new_path, path_root, len_root);
    memcpy(new_path + len_root, request->path, len_path);

    free(request->path);
    request->path = new_path;

    return 0;
}

int handle_client(int client_fd, char *path_root) {
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

    normalize_request_path(request, path_root);

    if (strcmp(request->method, "GET") == 0) {
        get_request(request, client_fd);
    } else {
        char *mess_header = "HTTP/1.1 501 Not Implemented\r\n";
        send(client_fd, mess_header, strlen(mess_header), MSG_CONFIRM);
    }

    close(client_fd);
    free_request(request);
    return 0;
}

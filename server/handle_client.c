#include <stdio.h>
#include <time.h>

#include "include/http-server.h"

#define BUFF_SIZE 1024

int send_date(int client_fd) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char s[33] = { 0 };
    char *str_date = "Date: ";
    int len_strdate = strlen(str_date);

    size_t ret = strftime(s, sizeof(s), "%a, %d %b %Y %T %z%n",tm);
    char *date = calloc(33 + len_strdate, sizeof(char));

    if (ret == 0) {
        perror("strftime()");
        return 1;
    }

    if (!date) {
        perror("calloc() send_date");
        return 1;
    }

    strncpy(date, str_date, len_strdate);
    memcpy(date + len_strdate, s, 33);

    send(client_fd, date, strlen(date), 0);
    free(date);
    return 0;
}

int get_request(request_t *request, int client_fd, char *path_root) {
    size_t nbytes = 0;
    char response[BUFF_SIZE];
    FILE *file = fopen(request->path, "rb");

    if (!file) {
        if (errno == ENOENT) {
            printf("Erreur 404\n");
            send(client_fd, "HTTP/1.0 404 Not Found\n", 24, 0);
            return 404;
        }

        if (errno == EACCES) {
            printf("Erreur 403\n");
            send(client_fd, "HTTP/1.0 403 Forbidden\n", 24, 0);
            return 403;
        }

        printf("errno %i : %s\n", errno, strerror(errno));
        return errno;
    }
    // Send response
    send(client_fd, "HTTP/1.0 200 OK\n", 17, 0);

    // Send headers
    send_date(client_fd);

    // Send file
    // PB: From times to times the html is not send if its the same one multiple time
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
        get_request(request, client_fd, path_root);
    } else {
        char *mess_header = "HTTP/1.0 501 Not Implemented\n";
        send(client_fd, mess_header, strlen(mess_header), MSG_CONFIRM);
    }

    close(client_fd);
    free_request(request);
    return 0;
}

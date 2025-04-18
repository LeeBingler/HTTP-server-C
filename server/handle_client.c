#include <stdio.h>
#include <netinet/in.h>

#include "include/http-server.h"
#include "include/http-headers.h"
#include "include/parse_request.h"
#include "include/status_log.h"


#define BUFF_SIZE 1024

int send_file(int client_fd, FILE *file) {
    size_t nbytes = 0;
    char response[BUFF_SIZE] = { 0 };

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

    return 0;
}

int get_request(request_t *request, int client_fd) {
    FILE *file = fopen(request->path, "rb");

    if (!file) {
        if (errno == ENOENT || errno == ENOTDIR) {
            send(client_fd, "HTTP/1.1 404 Not Found\r\n", 25, 0);
            return 404;
        }

        if (errno == EACCES) {
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
    send_contentlength(client_fd, file);
    send_contenttype(client_fd, request->path);
    send_connection(client_fd);
    send(client_fd, "\r\n", 2, 0);  // end of headers

    // Send file
    send_file(client_fd, file);

    fclose(file);
    return 200;
}

int normalize_request_path(request_t *request, char *path_root) {
    int len_path = strlen(request->path);
    int len_root = strlen(path_root);

    // verify that path is secure
    if (strstr(request->path, "..")) {
        return 403;
    }

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

int handle_client(int client_fd, struct sockaddr_in client_addr, char *path_root) {
    int status_code = 200;
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

    if ((status_code = normalize_request_path(request, path_root))) {
        if (status_code == 403) send(client_fd, "HTTP/1.1 403 Forbidden\r\n\r\n", 28, 0);
        free_request(request);
        return status_code;
    }

    if (strcmp(request->method, "GET") == 0) {
        status_code = get_request(request, client_fd);
    } else {
        status_code = 501;
        char *mess_header = "HTTP/1.1 501 Not Implemented\r\n";
        send(client_fd, mess_header, strlen(mess_header), 0);
    }

    status_log(request, client_addr, status_code);

    close(client_fd);
    free_request(request);
    return status_code;
}

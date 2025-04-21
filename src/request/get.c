#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#include "../../include/http/http-headers.h"
#include "../../include/http/parse_request.h"
#include "../../include/http/send_file.h"

int get(request_t *request, int client_fd) {
    size_t len_path = strlen(request->path);

    // make index.html default if the path end by "/"
    if (request->path[len_path - 1] == '/') {
        const char *index = "index.html";
        size_t new_len = len_path + strlen(index) + 1;

        char *new_path = calloc(new_len, sizeof(char));
        if (!new_path) return -1;

        strncpy(new_path, request->path, len_path);
        strcat(new_path, index);

        free(request->path);
        request->path = new_path;
    }

    FILE *file = fopen(request->path, "rb");

    if (!file) {
        if (errno == ENOENT || errno == ENOTDIR) {
            send(client_fd, "HTTP/1.1 404 Not Found\r\n", 25, 0);
            return 404;
        } else if (errno == EACCES) {
            send(client_fd, "HTTP/1.1 403 Forbidden\r\n", 25, 0);
            return 403;
        }

        send(client_fd, "HTTP/1.1 500 Internal Server Error\r\n", 36, 0);
        perror("fopen get");
        return 500;
    }

    // Send response
    send(client_fd, "HTTP/1.1 200 OK\r\n", 17, 0);
    send_date(client_fd);
    send_contentlength(client_fd, file);
    send_contenttype(client_fd, request->path);
    send_connection(client_fd, request->keep_alive);
    send(client_fd, "\r\n", 2, 0);  // end of headers

    send_file(client_fd, file);
    fclose(file);


    return 200;
}
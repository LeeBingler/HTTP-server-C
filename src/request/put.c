#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

#include "../../include/http/parse_request.h"
#include "../../include/http/http-headers.h"

int put(request_t *request, int client_fd) {
    if (!request->datas || strlen(request->datas) < 1) {
        send(client_fd, "HTTP/1.1 400 Bad Request\r\n", 28, 0);
        return 400;
    }

    // prevent rewrite on dir
    struct stat st;
    if (stat(request->path, &st) == 0 && S_ISDIR(st.st_mode)) {
        send(client_fd, "HTTP/1.1 400 Bad Request\r\n\r\n", 30, 0);
        return 400;
    }

    // prevent erase existing file
    if (access(request->path, F_OK) == 0) {
        send(client_fd, "HTTP/1.1 409 Conflict\r\n", 25, 0);
        return 409;
    }


    FILE *file = fopen(request->path, "w");

    if (!file) {
        if (errno == EACCES) {
            send(client_fd, "HTTP/1.1 403 Forbidden\r\n", 25, 0);
            return 403;
        }

        send(client_fd, "HTTP/1.1 500 Internal Server Error\r\n", 36, 0);
        perror("fopen put");
        return 500;
    }

    fputs(request->datas, file);

    send(client_fd, "HTTP/1.1 201 Created\r\n", 23, 0);
    send_date(client_fd);
    send_contenttype(client_fd, request->path);
    send_connection(client_fd, request->keep_alive);
    send_contentlocation(client_fd, strchr(request->path, '/'));
    send(client_fd, "\r\n", 2, 0);  // end of headers

    fclose(file);


    return 200;
}
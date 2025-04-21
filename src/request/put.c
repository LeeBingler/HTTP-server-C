#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

#include "../../include/http/parse_request.h"
#include "../../include/http/http-headers.h"

int put(request_t *request, int client_fd) {
    if (!request->datas || strlen(request->datas) < 1) {
        send(client_fd, "HTTP/1.1 400 Bad Request\r\n", 28, 0);
        return 400;
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

    send(client_fd, "HTTP/1.1 200 OK\r\n", 17, 0);
    send_date(client_fd);
    send_contentlength(client_fd, file);
    send_contenttype(client_fd, request->path);
    send_connection(client_fd, request->keep_alive);
    send_contentlocation(client_fd, strchr(request->path, '/'));
    send(client_fd, "\r\n", 2, 0);  // end of headers

    fclose(file);


    return 200;
}
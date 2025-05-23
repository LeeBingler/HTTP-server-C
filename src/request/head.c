#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>

#include "../../include/http/http-headers.h"
#include "../../include/http/parse_request.h"
#include "../../include/http/send_file.h"

int head(request_t *request, int client_fd) {
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

    fclose(file);


    return 200;
}
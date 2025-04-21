#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#include "../../include/http/http-headers.h"
#include "../../include/http/handle_request.h"

int post(request_t *request, int client_fd) {
    if (access(request->path, F_OK) != 0) {
        send(client_fd, "HTTP/1.1 404 Not Found\r\n", 25, 0);
        return 404;
    }

    if (access(request->path, R_OK) != 0) {
        send(client_fd, "HTTP/1.1 403 Forbidden\r\n", 25, 0);
        return 403;
    }

    if (!request->datas) {
        const char *msg = "HTTP/1.1 400 Bad Request\r\n";
        send(client_fd, msg, strlen(msg), 0);
        return 400;
    }

    send(client_fd, "HTTP/1.1 200 OK\r\n", 17, 0);
    send_date(client_fd);
    send_contentlength(client_fd, NULL);
    send_contenttype(client_fd, "txt");
    send_connection(client_fd, request->keep_alive);
    send(client_fd, "\r\n", 2, 0);  // end of headers

    send(client_fd, request->datas, strlen(request->datas), 0);


    return 200;
}
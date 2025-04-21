#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

#include "../../include/http/parse_request.h"
#include "../../include/http/http-headers.h"

int delete(request_t *request, int client_fd) {
    // prevent delete dir
    struct stat st;
    if (stat(request->path, &st) == 0 && S_ISDIR(st.st_mode)) {
        send(client_fd, "HTTP/1.1 400 Bad Request\r\n\r\n", 30, 0);
        return 400;
    }

    // prevent delete a file that does not exist
    if (access(request->path, F_OK) != 0) {
        send(client_fd, "HTTP/1.1 404 Not Found\r\n\r\n", 28, 0);
        return 404;
    }

    if (remove(request->path) != 0) {
        perror("remove()");
        send(client_fd, "HTTP/1.1 500 Internal Server Error\r\n\r\n", 38, 0);
        return 500;
    }

    send(client_fd, "HTTP/1.1 204 No Content\r\n", 25, 0);
    send_date(client_fd);
    send_connection(client_fd, request->keep_alive);
    send(client_fd, "\r\n", 2, 0);  // end of headers


    return 204;
}
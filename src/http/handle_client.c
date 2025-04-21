#include <stdio.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../include/http/http-headers.h"
#include "../../include/http/parse_request.h"
#include "../../include/http/status_log.h"
#include "../../include/http/handle_client.h"
#include "../../include/request/get.h"
#include "../../include/request/head.h"
#include "../../include/request/post.h"
#include "../../include/request/put.h"

int normalize_request_path(request_t *request, char *path_root) {
    if (strstr(request->path, "..")) {
        return 403; // Prevent directory traversal
    }

    // add path_root to path
    size_t len_path = strlen(request->path);
    size_t len_root = strlen(path_root);
    char *full_path = calloc(len_path + len_root + 1, sizeof(char));
    if (!full_path) return -1;

    memcpy(full_path, path_root, len_root);
    memcpy(full_path + len_root, request->path, len_path);

    free(request->path);
    request->path = full_path;


    return 200;
}

int handle_client(int client_fd, struct sockaddr_in client_addr, char *path_root) {
    char buff[BUFF_SIZE];
    int status_code = 200;
    int keep_alive;

    struct timeval timeout = { .tv_sec = 5, .tv_usec = 0 };
    if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        close(client_fd);
        return -1;
    }

    while (1) {
        memset(buff, 0, BUFF_SIZE);
        int byte_recv = recv(client_fd, buff, BUFF_SIZE, 0);

        if (byte_recv < 0 ) {
            if (errno == EINTR) continue; // signal interrupt
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("Timeout: client inactive for too long\n");
                break;
            }
            perror("recv()");
            break;

        } else if (byte_recv == 0) {
            printf("Client disconnected\n");
            break;
        }

        request_t *request = parse_request(buff);
        if (!request) {
            status_code = 400;
            break;
        }

        if (strcmp(request->protocol, "HTTP/1.1") == 0 && request->host == NULL) {
            status_code = 400;
            send(client_fd, "HTTP/1.1 400 Bad Request\r\n\r\n", 30, 0);
            status_log(request, client_addr, status_code);
            free_request(request);
            break;
        }

        status_code = normalize_request_path(request, path_root);
        if (status_code != 200) {
            if (status_code == 403) {
                send(client_fd, "HTTP/1.1 403 Forbidden\r\n\r\n", 28, 0);
            }
            status_log(request, client_addr, status_code);
            free_request(request);
            break;
        }


        if (strcmp(request->method, "GET") == 0) {
            status_code = get(request, client_fd);

        } else if (strcmp(request->method, "POST") == 0) {
            status_code = post(request, client_fd);

        } else if (strcmp(request->method, "HEAD") == 0) {
            status_code = head(request, client_fd);

        } else if (strcmp(request->method, "PUT") == 0) {
            status_code = put(request, client_fd);

        } else {
            const char *mess_header = "HTTP/1.1 501 Not Implemented\r\nAllow: GET POST HEAD\r\n";
            send(client_fd, mess_header, strlen(mess_header), 0);
            status_code = 501;
        }

        status_log(request, client_addr, status_code);

        keep_alive = request->keep_alive;
        free_request(request);
        if (!keep_alive) break;
    }

    close(client_fd);
    return status_code;
}

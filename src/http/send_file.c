#include <stdio.h>
#include <netinet/in.h>
#include <errno.h>

#include "../../include/config.h"

int send_file(int client_fd, FILE *file) {
    size_t bytes_read = 0;
    char response[CHUNK_SIZE] = { 0 };

    while ((bytes_read = fread(response, sizeof(char), CHUNK_SIZE, file)) > 0) {
        size_t total_sent = 0;

        while (total_sent < bytes_read) {
            ssize_t sent = send(client_fd, response + total_sent, bytes_read - total_sent, 0);

            if (sent < 0) {
                if (errno == EINTR) continue;
                return -1;
            }

            total_sent += sent;
        }
    }


    return 0;
}
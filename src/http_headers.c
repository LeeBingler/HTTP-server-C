#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

#include "../include/mime_types.h"
#include "../include/utils.h"

int send_date(int client_fd) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char s[33] = { 0 };
    char *prefix = "Date: ";
    char *suffix = "\r\n";
    size_t ret = strftime(s, sizeof(s), "%a, %d %b %Y %T %z",tm);
    size_t total_len = strlen(s) + strlen(prefix) + strlen(suffix);
    char *date = calloc(total_len + 1, sizeof(char));

    if (ret == 0) {
        perror("strftime()");
        return 1;
    }

    if (!date) {
        perror("calloc() send_date");
        return 1;
    }

    strcpy(date, prefix);
    strcat(date, s);
    strcat(date, suffix);

    send(client_fd, date, total_len, 0);
    free(date);
    return 0;
}

int send_contentlength(int client_fd, FILE *file) {
    long file_size;
    char message[64] = { 0 };
    int len_message = 0;

    // Get size of file
    fseek(file, 0L, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    len_message = snprintf(message, sizeof(message), "Content-Lenght: %ld\r\n", file_size);
    send(client_fd, message, len_message, 0);
    return 0;
}

int send_contenttype(int client_fd, char *filename) {
    const char *ext = get_file_extension(filename);
    const char *mime_type = get_mime_type(ext);
    const char *prefix = "Content-Type: ";
    const char *suffix = "\r\n";

    size_t total_len = strlen(prefix) + strlen(mime_type) + strlen(suffix);

    char *message = malloc(total_len + 1);
    if (!message) {
        perror("malloc");
        return -1;
    }

    strcpy(message, prefix);
    strcat(message, mime_type);
    strcat(message, suffix);

    send(client_fd, message, total_len, 0);
    free(message);
    return 0;
}

int send_connection(int client_fd) {
    char *message = "Connection: close\r\n";

    send(client_fd, message, strlen(message), 0);
    return 0;
}
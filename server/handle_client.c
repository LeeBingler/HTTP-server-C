#include <stdio.h>
#include <time.h>

#include "include/http-server.h"

#define BUFF_SIZE 1024

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

int send_contentlenght(int client_fd, FILE *file) {
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

char* get_file_extension(char *filename) {
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

char* get_mime_type(char *ext) {
    if (strcmp(ext, "html") == 0) return "text/html";
    if (strcmp(ext, "htm") == 0)  return "text/html";
    if (strcmp(ext, "css") == 0)  return "text/css";
    if (strcmp(ext, "js") == 0)   return "application/javascript";
    if (strcmp(ext, "jpg") == 0)  return "image/jpeg";
    if (strcmp(ext, "jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, "png") == 0)  return "image/png";
    if (strcmp(ext, "gif") == 0)  return "image/gif";
    if (strcmp(ext, "txt") == 0)  return "text/plain";
    if (strcmp(ext, "pdf") == 0)  return "application/pdf";

    return "application/octet-stream";
}

int send_contenttype(int client_fd, char *filename) {
    char *ext = get_file_extension(filename);
    char *mime_type = get_mime_type(ext);
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

int get_request(request_t *request, int client_fd) {
    size_t nbytes = 0;
    char response[BUFF_SIZE];
    FILE *file = fopen(request->path, "rb");

    if (!file) {
        if (errno == ENOENT) {
            printf("Erreur 404\n");
            send(client_fd, "HTTP/1.1 404 Not Found\r\n", 25, 0);
            return 404;
        }

        if (errno == EACCES) {
            printf("Erreur 403\n");
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
    send_contentlenght(client_fd, file);
    send_contenttype(client_fd, request->path);
    send_connection(client_fd);

    send(client_fd, "\r\n", 2, 0);  // end of headers

    // Send file
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

    fclose(file);
    return 0;
}

int normalize_request_path(request_t *request, char *path_root) {
    int len_path = strlen(request->path);
    int len_root = strlen(path_root);

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

int handle_client(int client_fd, char *path_root) {
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

    normalize_request_path(request, path_root);

    if (strcmp(request->method, "GET") == 0) {
        get_request(request, client_fd);
    } else {
        char *mess_header = "HTTP/1.1 501 Not Implemented\r\n";
        send(client_fd, mess_header, strlen(mess_header), MSG_CONFIRM);
    }

    close(client_fd);
    free_request(request);
    return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>

#define BUFF_SIZE 1024

typedef struct HttpHeader {
    char *key;
    char *value;
    struct HttpHeader *next;
} HttpHeader_t;

typedef struct {
    char *method;
    char *path;
    char *protocol;
    HttpHeader_t *headers;
    int header_count;
    char *datas;
} request_t;

void free_request(request_t *request) {
    if (request == NULL) return;

    if (request->method != NULL) free(request->method);
    if (request->path != NULL) free(request->path);
    if (request->protocol != NULL) free(request->protocol);
    if (request->datas != NULL) free(request->datas);

    if (request->headers != NULL) {
        HttpHeader_t *ptr = request->headers;
        while (ptr != NULL) {
            ptr = ptr->next;
            free(request->headers->key);
            free(request->headers->value);
            free(request->headers);
            request->headers = ptr;
        }
    }

    free(request);
}

int parse_fl(char *line, request_t *request) {
    char **map = malloc(sizeof(char *) * 3);

    if (!line || !request || !map) {
        perror("First line request invalid");
        return errno;
    }

    int counter = 0;
    int start = 0;
    int len = strlen(line);
    for (int i = 0; i <= len; i++) {
        if (line[i] == ' ' || line[i] == '\0') {
            line[i] = '\0';
            map[counter] = strndup(line + start, i - start);
            start = i + 1;
            counter++;
        }
    }

    if (!map[0] || !map[1] || !map[2]) {
        perror("First line request invalid");
        free(request);
        return errno;
    }

    request->method = map[0];
    request->path = map[1];
    request->protocol = map[2];
    free(map);
    return 0;
}

int parse_headers(request_t *request) {
    char *line;

    while ((line = strtok(NULL, "\n")) && strlen(line) > 2) {
        HttpHeader_t *node = calloc(1, sizeof(HttpHeader_t));
        char *two_point = strchr(line, ':');

        if (two_point) {
            *two_point = '\0';
            char *key = line;
            char *value = two_point + 1;
            HttpHeader_t *ptr = request->headers;
            while (*value == ' ') value++;

            node->key = strdup(key);
            node->value = strdup(value);

            if (ptr == NULL) {
                request->headers = node;
                request->header_count++;
            } else {
                while (ptr->next != NULL)
                ptr = ptr->next;

                ptr->next = node;
                request->header_count++;
            }
        } else {
            break;
        }
    }

    return 0;
}

int parse_data(request_t *request) {

    return 0;
}

request_t *parse_request(char *raw_request) {
    request_t *request = calloc(1, sizeof(request_t));
    char *line = strtok(raw_request, "\r\n");

    if (request == NULL) {
        perror("calloc() in parse_request");
        return request;
    }

    // Parse first line
    if (parse_fl(line, request)) {
        free(request);
        return NULL;
    }

    // Parse Header
    if (parse_headers(request)) {
        free(request);
        return NULL;
    }

    if (1) {
        printf("method : %s path : %s protocol : %s \n\n", request->method, request->path, request->protocol);
        printf("headers : \n\n");
        HttpHeader_t *ptr = request->headers;
        while (ptr != NULL) {
            printf("%s : %s\n", ptr->key, ptr->value);
            ptr = ptr->next;
        }
    }

    return request;
}

int handle_client(int client_fd) {
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
    printf("raw request : \n\n%s\n\n", buff);
    request_t *request = parse_request(buff);
    if (request == NULL)
        return errno;

    char *mess_header = "HTTP/1.1 200 OK\n"; // Content-type + Date + connection + transfer-encoding
    send(client_fd, mess_header, strlen(mess_header), MSG_CONFIRM);
    printf("send\n");

    close(client_fd);
    free_request(request);
    return 0;
}

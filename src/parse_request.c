#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "../include/parse_request.h"

#define ERR_EXIT(msg) (perror(msg), errno)

void free_request(request_t *request) {
    if (!request) return;

    if (request->method != NULL) free(request->method);
    if (request->path != NULL) free(request->path);
    if (request->protocol != NULL) free(request->protocol);
    if (request->datas != NULL) free(request->datas);

    if (request->headers != NULL) {
        HttpHeader_t *ptr = request->headers;
        while (ptr) {
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

    if (!line || !request || !map) return ERR_EXIT("First line request invalid");

    // Put each word separated by a space in map
    int counter = 0, start = 0, len = strlen(line);
    for (int i = 0; i <= len; i++) {
        if (line[i] == ' ' || line[i] == '\0') {
            line[i] = '\0';
            map[counter++] = strndup(line + start, i - start);
            start = i + 1;
        }
    }

    if (!map[0] || !map[1] || !map[2]) return ERR_EXIT("First line request invalid");

    request->method = map[0];
    request->path = map[1];
    request->protocol = map[2];
    free(map);
    return 0;
}

int parse_headers(request_t *request) {
    char *line;

    while ((line = strtok(NULL, "\n")) && strlen(line) > 2) {
        HttpHeader_t *node;
        char *colon = strchr(line, ':');

        if (!colon) return 1;

        *colon = '\0';
        char *key = line;
        char *value = colon + 1;
        while (*value == ' ') value++;

        node = calloc(1, sizeof(HttpHeader_t));
        if (!node) return 1;

        node->key = strdup(key);
        node->value = strdup(value);
        if (!node->key || !node->value) {
            free(node->key);
            free(node->value);
            free(node);
            return 1;
        }

        if (!request->headers) {
            request->headers = node;
        } else {
            HttpHeader_t *ptr = request->headers;
            while (ptr->next) ptr = ptr->next;
            ptr->next = node;
        }

        request->header_count++;

        if (strcmp(node->key, "Host") == 0) request->host = node;
    }

    return 0;
}

int parse_datas(request_t *request) {
    /*
     * Should use a parser like cJSON to make the request data cleaner
     */
    char *line = strtok(NULL, "\n\r");
    if (!line) return 0;

    request->datas = strdup(line);
    return request->datas ? 0 : 1;
}

request_t *parse_request(char *raw_request) {
    request_t *request = calloc(1, sizeof(request_t));
    char *line = strtok(raw_request, "\r\n");

    if (!request) {
        perror("calloc request");
        return NULL;
    }

    if (parse_fl(line, request) || parse_headers(request) || parse_datas(request)) {
        free_request(request);
        return NULL;
    }

    return request;
}

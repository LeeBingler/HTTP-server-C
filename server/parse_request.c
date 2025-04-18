#include "include/http-server.h"

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

    // Put each word separated by a space in map
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

        if (!two_point || !node)
            return 1;

        *two_point = '\0';
        char *key = line;
        char *value = two_point + 1;
        HttpHeader_t *ptr = request->headers;
        while (*value == ' ') value++;

        node->key = strdup(key);

        if (!(node->key)) {
            free(node);
            return 1;
        }

        node->value = strdup(value);

        if (!(node->value)) {
            free(node->key);
            free(node);
            return 1;
        }

        if (ptr == NULL) {
            request->headers = node;
            request->header_count++;
        } else {
            while (ptr->next != NULL)
                ptr = ptr->next;

            ptr->next = node;
            request->header_count++;
        }
    }

    return 0;
}

int parse_datas(request_t *request) {
    /*
     * Should use a parser like cJSON to make the request data cleaner
     */
    char *line = strtok(NULL, "\n\r");

    if (!line)
        return 0;

    request->datas = strdup(line);

    if (!request->datas)
        return 1;
    return 0;
}

request_t *parse_request(char *raw_request) {
    request_t *request = calloc(1, sizeof(request_t));
    char *line = strtok(raw_request, "\r\n");

    if (request == NULL) {
        perror("calloc() in parse_request");
        return request;
    }

    if (parse_fl(line, request)) {
        free_request(request);
        return NULL;
    }

    if (parse_headers(request)) {
        free_request(request);
        return NULL;
    }

    if(parse_datas(request)) {
        free_request(request);
        return NULL;
    }

    return request;
}

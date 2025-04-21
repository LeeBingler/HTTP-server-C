#pragma once

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
    HttpHeader_t *host;
    int header_count;
    char *datas;
    int keep_alive;
} request_t;

request_t *parse_request(char *request);
void free_request(request_t *request);
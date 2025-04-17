#pragma once

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>

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

// parse request
request_t *parse_request(char *request);
void free_request(request_t *request);


int handle_client(int client_fd);

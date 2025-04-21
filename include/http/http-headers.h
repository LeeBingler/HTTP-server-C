#ifndef HTTP_HEADERS_H
#define HTTP_HEADERS_H

#include <stdio.h>

int send_date(int client_fd);
int send_contentlength(int client_fd, FILE *file);
int send_contenttype(int client_fd, char *filename);
int send_connection(int client_fd, int keep_alive);
int send_contentlocation(int client_fd, char *filename);

#endif
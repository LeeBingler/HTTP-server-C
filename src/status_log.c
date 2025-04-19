#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>

#include "../include/parse_request.h"

void print_timestamp() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "[%Y-%m-%d %H:%M:%S]", t);
    printf("%s ", buf);
}

int status_log(request_t *request, struct sockaddr_in client_addr, int status_code) {
    print_timestamp();
    printf("%s \"%s %s %s\" %d\n", inet_ntoa(client_addr.sin_addr), request->method, request->path, request->protocol,
        status_code);
    return 0;
}
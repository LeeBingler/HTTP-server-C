#pragma once

void print_timestamp(void);
int status_log(request_t *request, struct sockaddr_in client_addr, int status_code);
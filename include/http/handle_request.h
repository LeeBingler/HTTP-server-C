#pragma once

#include "parse_request.h"

int get_request(request_t *request, int client_fd);
int post_request(request_t *request, int client_fd);
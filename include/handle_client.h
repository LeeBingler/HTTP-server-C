#pragma once

#include <sys/socket.h>

int handle_client(int client_fd, struct sockaddr_in client_addr, char *path_root);
#pragma once

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>


int handle_client(int client_fd, struct sockaddr_in client_addr, char *path_root);
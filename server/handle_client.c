#include <dirent.h>

#include "include/http-server.h"

#define BUFF_SIZE 1024

int get_request(request_t *request, int client_fd, char *path_root) {
    struct dirent *entry;
    int len_path = strlen(request->path);
    DIR *root_dir = opendir(path_root);

    if (!root_dir) {
        perror("opendir");
        return 1;
    }

    // make index.html default if the path end by "/"
    if (request->path[len_path - 1] == '/') {
        char *str_index = "index.html";
        int len_index = strlen(str_index);
        char *new_path = calloc(len_path + len_index + 1, sizeof(char));

        strncpy(new_path, request->path, len_path);
        strncat(new_path, str_index, len_index);
        free(request->path);

        request->path = new_path;
    }

    printf("%s\n", request->path);
    while ((entry = readdir(root_dir)) != NULL) {
        if (!strcmp (entry->d_name, ".") || !strcmp (entry->d_name, ".."))
            continue;

        if (strcmp(entry->d_name, request->path) == 0) {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(root_dir);
    return 0;
}

int handle_client(int client_fd, char *path_root) {
    char buff[BUFF_SIZE] = { 0 };
    int byte_recv = recv(client_fd, buff, BUFF_SIZE, MSG_DONTWAIT);

    if (byte_recv == -1) {
        perror("recv()");
        close(client_fd);
        return errno;

    } else if (byte_recv == 0) {
        printf("Client disconnected\n");
        close (client_fd);
        return 0;
    }

    request_t *request = parse_request(buff);
    if (request == NULL)
        return errno;

    if (strcmp(request->method, "GET") == 0) {
        get_request(request, client_fd, path_root);
    } else {
        char *mess_header = "HTTP/1.0 501 Not Implemented\n";
        send(client_fd, mess_header, strlen(mess_header), MSG_CONFIRM);
    }

    printf("send\n");

    close(client_fd);
    free_request(request);
    return 0;
}

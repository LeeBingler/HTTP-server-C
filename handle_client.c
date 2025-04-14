#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>

int parse_request(char *request) {
    printf("%s", request);
    return 0;
}

int handle_client(int client_fd) {
    char buff[1080] = { 0 };
    int byte_recv = recv(client_fd, buff, 1080, MSG_DONTWAIT);
    
    if (byte_recv == -1) {
        perror("recv()");
        close(client_fd);
        return 1;

    } else if (byte_recv == 0) {
        printf("User disconnected");

    } else if (byte_recv > 0) {
        parse_request(buff);
        char header[17] = "HTTP/1.1 200 OK\n";
        send(client_fd, header, 17, MSG_CONFIRM);
        printf("send\n");
    }

    close(client_fd);

    return 0;
}

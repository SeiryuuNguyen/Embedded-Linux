#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/my_unix_socket"
#define BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
    if(argc != 2){
        fprintf(stderr, "Example: %s \"Hello from server to client!\"\n", argv[0]);
        exit(-1);
    }
    int server_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE] = {0};

    server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(server_fd < 0){
        perror("socket error");
        exit(-1);
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    unlink(SOCKET_PATH);

    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) < 0){
        perror("bind error");
        exit(-1);
    }
    printf("UDP Server is bound to %s\n", SOCKET_PATH);

    client_len = sizeof(struct sockaddr_un);
    recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_len);
    printf("Client says: %s\n", buffer);

    sendto(server_fd, argv[1], sizeof(argv[1]), 0, (struct sockaddr*)&client_addr, client_len);

    close(server_fd);
    unlink(SOCKET_PATH);
}
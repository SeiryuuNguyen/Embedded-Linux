#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_PATH "/tmp/my_unix_socket"
#define CLIENT_PATH "/tmp/my_uinx_client_socket"
#define BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
    if(argc != 2){
        fprintf(stderr, "Example: %s \"Hello from server to client!\"\n", argv[0]);
        exit(-1);
    }
    int sock_fd;
    struct sockaddr_un server_addr, client_addr;
    char buffer[BUFFER_SIZE] = {0};

    sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(sock_fd < 0){
        perror("socket error");
        exit(-1);
    }

    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, CLIENT_PATH);
    unlink(CLIENT_PATH);
    if(bind(sock_fd, (struct sockaddr*)&client_addr, sizeof(struct sockaddr_un)) < 0){
        perror("bind error on client");
        exit(-1);
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SERVER_PATH);

    sendto(sock_fd, argv[1], sizeof(argv[1]), 0, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un));
    printf("Message sent to server\n");

    recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    printf("Server says: %s\n", buffer);

    close(sock_fd);
    unlink(CLIENT_PATH);
}
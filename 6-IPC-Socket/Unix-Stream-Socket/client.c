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
    int sock_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE] = {0};

    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sock_fd < 0){
        perror("socket error");
        exit(-1);
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCKET_PATH);

    if(connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_un)) < 0){
        perror("connect error");
        exit(-1);
    }
    printf("Connected to server\n");

    write(sock_fd, argv[1], sizeof(argv[1]));
    read(sock_fd, buffer, BUFFER_SIZE);
    printf("Server says: %s\n", buffer);
    close(sock_fd);
}
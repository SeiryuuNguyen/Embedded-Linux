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
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE] = {0};

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
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
    printf("Server is bound to %s\n", SOCKET_PATH);

    if(listen(server_fd, 5) < 0){
        perror("listening error");
        exit(-1);
    }
    printf("Server is listening... \n");

    client_len = sizeof(struct sockaddr_un);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if(client_fd < 0)
    {
        perror("accept error");
        exit(-1);
    }
    printf("Connection accepted\n");

    read(client_fd, buffer, BUFFER_SIZE);
    printf("Client says: %s\n", buffer);
    write(client_fd, argv[1], sizeof(argv[1]));

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
}
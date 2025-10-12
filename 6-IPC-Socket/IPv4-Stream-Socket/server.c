#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/un.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
    if(argc != 2){
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int port = atoi(argv[1]);
    if(port <= 0 || port > 65535)
    {
        fprintf(stderr, "Invalid port number: %s\n", argv[1]);
        exit(1);
    }
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE] = {0};
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        printf("socket failed");
        exit(1);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr*)&address, addrlen) < 0){
        printf("bind failed");
        exit(1);
    }

    if(listen(server_fd, 1) < 0){
        printf("listen failed");
        exit(1);
    }

    printf("IPv4 Stream Server listening on port: %d\n", port);

    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if(new_socket < 0){
        printf("accept failed");
        exit(1);
    }

    printf("Connection accepted\n");
    read(new_socket, buffer, BUFFER_SIZE);

    printf("Client sent: %s\n", buffer);

    char *response = "Hello from IPv4 Stream Server";
    send(new_socket, response, strlen(response), 0);

    close(new_socket);
    close(server_fd);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int port = atoi(argv[1]);
    if(port <= 0 || port > 65535){
        fprintf(stderr, "Invalid port number: %s\n", argv[1]);
        exit(1);
    }
    int sock_fd = 0;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    const char *hello = "Hello from client";
    
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0){
        printf("Socket creation error\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, "127.0.0.1",&server_addr.sin_addr) < 0){
        printf("Invalid address/ Address not supported \n");
        exit(1);
    }

    if(connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Connection Failed\n");
        exit(1);
    }

    printf("Connected to server\n");

    send(sock_fd, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    read(sock_fd, buffer, BUFFER_SIZE);
    printf("Server replied: %s\n", buffer);

    close(sock_fd);
    
}
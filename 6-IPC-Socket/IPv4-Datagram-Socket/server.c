#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    if(argc != 3){
        fprintf(stderr, "Usage: %s <port> \"<message_to_client>\"\n", argv[0]);
        fprintf(stderr, "Example: %s 12345 \"Hello from server to client!\"\n", argv[0]);
        exit(-1);
    }
    int port = atoi(argv[1]);
    if(port <= 0 || port > 65535){
        fprintf(stderr, "Invalid port number: %s\n", argv[1]);
        exit(-1);
    }

    int sock_fd;
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in server_addr, client_addr;

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0){
        perror("socket creation failed");
        exit(-1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if(bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("bind failed");
        exit(-1);
    }

    printf("UDP Server listening on port: %d\n", port);

    socklen_t len = sizeof(client_addr);
    int n;
    n = recvfrom(sock_fd, (char*)buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &len);
    buffer[n] = '\0';
    printf("Clinet: %s\n", buffer);
   printf("Connection info: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    sendto(sock_fd, (const char*)argv[2], strlen(argv[2]), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
    printf("Sent a message to the client");
    close(sock_fd);
}


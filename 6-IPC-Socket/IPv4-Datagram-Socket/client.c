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
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0){
        perror("socket creation failed");
        exit(-1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t len;
    sendto(sock_fd, (const char *)argv[2], strlen(argv[2]), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("Sent a message to the server\n");

    int n;
    n = recvfrom(sock_fd, (char*)buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &len);
    buffer[n] = '\0';
    printf("Server: %s\n", buffer);

    close(sock_fd);
}
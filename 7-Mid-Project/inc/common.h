#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <stdbool.h>
#include <errno.h>

#define MAX_CONNECTIONS 100
#define MAX_MESSAGE_LEN 100

typedef struct Connection {
    int id;
    int sock;
    char ip[INET_ADDRSTRLEN];
    int port;
    pthread_t thread;
    struct Connection *next;
} Connection;

// Biến toàn cục được khai báo extern
extern Connection *head;
extern pthread_mutex_t mutex;
extern int next_id;
extern char my_ip[INET_ADDRSTRLEN];
extern int my_port;
extern int server_sock;
extern volatile int running;

// Hàm cleanup
void signal_cleanup();

#endif
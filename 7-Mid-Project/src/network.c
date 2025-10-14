#include "network.h"
#include "connection.h"

char my_ip[INET_ADDRSTRLEN];
int my_port;
int server_sock;
volatile int running = 1;

void get_local_ip(char *buf) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        strcpy(buf, "127.0.0.1");
        return;
    }
    
    const char *google_dns = "8.8.8.8";
    int dns_port = 53;
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(google_dns);
    serv.sin_port = htons(dns_port);

    int err = connect(sock, (const struct sockaddr *)&serv, sizeof(serv));
    if (err < 0) {
        strcpy(buf, "127.0.0.1");
        close(sock);
        return;
    }

    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (struct sockaddr *)&name, &namelen);
    if (err < 0) {
        strcpy(buf, "127.0.0.1");
        close(sock);
        return;
    }

    inet_ntop(AF_INET, &name.sin_addr, buf, INET_ADDRSTRLEN);
    close(sock);
}

int connect_to(const char *ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }

    return sock;
}

int initialize_server(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }

    if (listen(sock, 10) < 0) {
        perror("listen");
        close(sock);
        return -1;
    }

    return sock;
}

void *listener(void *arg) {
    // Đặt thread thành cancellable
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client = accept(server_sock, (struct sockaddr *)&client_addr, &len);
        
        if (client < 0) {
            if (!running) break;
            if (errno == EINTR || errno == EBADF) break;
            continue;
        }
        
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_addr.sin_port);

        // Check if already connected
        pthread_mutex_lock(&mutex);
        bool exists = false;
        for (Connection *cur = head; cur; cur = cur->next) {
            if (strcmp(cur->ip, client_ip) == 0 && cur->port == client_port) {
                exists = true;
                break;
            }
        }
        pthread_mutex_unlock(&mutex);
        
        if (exists) {
            close(client);
            continue;
        }

        Connection *new_con = (Connection *)malloc(sizeof(Connection));
        if (!new_con) {
            close(client);
            continue;
        }
        
        new_con->id = next_id++;
        new_con->sock = client;
        strcpy(new_con->ip, client_ip);
        new_con->port = client_port;
        pthread_create(&new_con->thread, NULL, recv_thread, new_con);
        add_to_list(new_con);
        printf("Accepted connection from %s:%d as ID %d\n", client_ip, client_port, new_con->id);
    }
    
    printf("Listener thread stopped\n");
    return NULL;
}
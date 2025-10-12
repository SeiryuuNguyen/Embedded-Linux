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

// Max connections for simplicity
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

Connection *head = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int next_id = 1;
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

void add_to_list(Connection *new_con) {
    pthread_mutex_lock(&mutex);
    new_con->next = head;
    head = new_con;
    pthread_mutex_unlock(&mutex);
}

Connection *find_connection(int id) {
    pthread_mutex_lock(&mutex);
    for (Connection *cur = head; cur; cur = cur->next) {
        if (cur->id == id) {
            pthread_mutex_unlock(&mutex);
            return cur;
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

bool remove_from_list(Connection *target) {
    pthread_mutex_lock(&mutex);
    Connection *prev = NULL;
    Connection *cur = head;
    while (cur) {
        if (cur == target) {
            if (prev) {
                prev->next = cur->next;
            } else {
                head = cur->next;
            }
            pthread_mutex_unlock(&mutex);
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    pthread_mutex_unlock(&mutex);
    return false;
}

void *recv_thread(void *arg) {
    Connection *c = (Connection *)arg;
    char buf[101];
    while (running) {
        ssize_t n = recv(c->sock, buf, 1, 0);
        if (n <= 0) break;
        uint8_t len = (uint8_t)buf[0];
        if (len > MAX_MESSAGE_LEN) break;
        n = recv(c->sock, buf, len, 0);
        if (n != len) break;
        buf[len] = '\0';
        printf("From %s:%d: %s\n", c->ip, c->port, buf);
    }
    printf("Connection ID %d closed.\n", c->id);
    if (remove_from_list(c)) {
        free(c);
    }
    return NULL;
}

void *listener(void *arg) {
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client = accept(server_sock, (struct sockaddr *)&client_addr, &len);
        if (client < 0) {
            if (!running) break;
            continue;
        }
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_addr.sin_port);

        // Check if already connected
        pthread_mutex_lock(&mutex);
        for (Connection *cur = head; cur; cur = cur->next) {
            if (strcmp(cur->ip, client_ip) == 0 && cur->port == client_port) {
                pthread_mutex_unlock(&mutex);
                close(client);
                continue;
            }
        }
        pthread_mutex_unlock(&mutex);

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
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }
    my_port = atoi(argv[1]);
    if (my_port <= 0 || my_port > 65535) {
        fprintf(stderr, "Invalid port\n");
        return 1;
    }
    get_local_ip(my_ip);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(my_port);

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_sock);
        return 1;
    }

    if (listen(server_sock, 10) < 0) {
        perror("listen");
        close(server_sock);
        return 1;
    }

    pthread_t listener_th;
    pthread_create(&listener_th, NULL, listener, NULL);

    char line[1024];
    while (running && fgets(line, sizeof(line), stdin)) {
        char *p = strchr(line, '\n');
        if (p) *p = '\0';

        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "help") == 0) {
            printf("help: Hiển thị danh sách tất cả các lệnh và mô tả ngắn gọn cách sử dụng chúng.\n");
            printf("myip: In ra địa chỉ IP của máy đang chạy chương trình.\n");
            printf("myport: In ra cổng (port) mà ứng dụng đang lắng nghe các kết nối đến.\n");
            printf("connect <destination_ip> <port>: Thiết lập một kết nối TCP mới đến một peer khác.\n");
            printf("list: Hiển thị danh sách tất cả các kết nối đang hoạt động.\n");
            printf("terminate <connection_id>: Ngắt một kết nối cụ thể dựa trên ID.\n");
            printf("send <connection_id> <message>: Gửi một tin nhắn đến một peer cụ thể.\n");
            printf("exit: Đóng tất cả các kết nối và thoát khỏi chương trình.\n");
        } else if (strcmp(cmd, "myip") == 0) {
            printf("%s\n", my_ip);
        } else if (strcmp(cmd, "myport") == 0) {
            printf("%d\n", my_port);
        } else if (strcmp(cmd, "connect") == 0) {
            char ip[INET_ADDRSTRLEN];
            int port;
            if (sscanf(line + strlen(cmd) + 1, "%15s %d", ip, &port) != 2) {
                printf("Usage: connect <ip> <port>\n");
                continue;
            }
            if (port < 1 || port > 65535) {
                printf("Invalid port\n");
                continue;
            }
            struct in_addr addr;
            if (inet_pton(AF_INET, ip, &addr) != 1) {
                printf("Invalid IP\n");
                continue;
            }
            if (strcmp(ip, my_ip) == 0 && port == my_port) {
                printf("Cannot connect to self\n");
                continue;
            }
            // Check existing
            bool exists = false;
            pthread_mutex_lock(&mutex);
            for (Connection *cur = head; cur; cur = cur->next) {
                if (strcmp(cur->ip, ip) == 0 && cur->port == port) {
                    exists = true;
                    break;
                }
            }
            pthread_mutex_unlock(&mutex);
            if (exists) {
                printf("Connection already exists\n");
                continue;
            }
            int sock = connect_to(ip, port);
            if (sock < 0) {
                printf("Failed to connect\n");
                continue;
            }
            Connection *new_con = (Connection *)malloc(sizeof(Connection));
            if (!new_con) {
                close(sock);
                continue;
            }
            new_con->id = next_id++;
            new_con->sock = sock;
            strcpy(new_con->ip, ip);
            new_con->port = port;
            pthread_create(&new_con->thread, NULL, recv_thread, new_con);
            add_to_list(new_con);
            printf("Connected to %s:%d as ID %d\n", ip, port, new_con->id);
        } else if (strcmp(cmd, "list") == 0) {
            pthread_mutex_lock(&mutex);
            printf("ID\tIP\t\tPort\n");
            for (Connection *cur = head; cur; cur = cur->next) {
                printf("%d\t%s\t%d\n", cur->id, cur->ip, cur->port);
            }
            pthread_mutex_unlock(&mutex);
        } else if (strcmp(cmd, "terminate") == 0) {
            int id;
            if (sscanf(line + strlen(cmd) + 1, "%d", &id) != 1) {
                printf("Usage: terminate <id>\n");
                continue;
            }
            pthread_mutex_lock(&mutex);
            Connection *c = NULL;
            Connection *temp = head;
            while (temp) {
                if (temp->id == id) {
                    c = temp;
                    break;
                }
                temp = temp->next;
            }
            if (!c) {
                pthread_mutex_unlock(&mutex);
                printf("Invalid connection ID\n");
                continue;
            }
            shutdown(c->sock, SHUT_RDWR);
            close(c->sock);
            pthread_t tid = c->thread;
            // Remove from list
            Connection *prev = NULL;
            temp = head;
            bool was_removed = false;
            while (temp) {
                if (temp == c) {
                    if (prev) {
                        prev->next = temp->next;
                    } else {
                        head = temp->next;
                    }
                    was_removed = true;
                    break;
                }
                prev = temp;
                temp = temp->next;
            }
            pthread_mutex_unlock(&mutex);
            pthread_join(tid, NULL);
            if (was_removed) free(c);
        } else if (strcmp(cmd, "send") == 0) {
            char *rest = line + strlen(cmd) + 1;
            int id;
            char *msg = strchr(rest, ' ');
            if (!msg || sscanf(rest, "%d", &id) != 1) {
                printf("Usage: send <id> <message>\n");
                continue;
            }
            msg++;  // skip space
            if (strlen(msg) > MAX_MESSAGE_LEN) {
                printf("Message too long (max 100 chars)\n");
                continue;
            }
            if (strlen(msg) == 0) continue;
            Connection *c = find_connection(id);
            if (!c) {
                printf("Invalid connection ID\n");
                continue;
            }
            uint8_t len = (uint8_t)strlen(msg);
            send(c->sock, &len, 1, 0);
            send(c->sock, msg, len, 0);
        } else if (strcmp(cmd, "exit") == 0) {
            running = 0;
        } else {
            printf("Unknown command. Use 'help' for list.\n");
        }
    }

    // Cleanup
    close(server_sock);
    pthread_join(listener_th, NULL);

    pthread_mutex_lock(&mutex);
    Connection *list_to_close = head;
    head = NULL;
    pthread_mutex_unlock(&mutex);

    Connection *cur = list_to_close;
    while (cur) {
        Connection *next = cur->next;
        shutdown(cur->sock, SHUT_RDWR);
        close(cur->sock);
        pthread_join(cur->thread, NULL);
        free(cur);
        cur = next;
    }

    return 0;
}
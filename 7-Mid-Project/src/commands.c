#include "commands.h"
#include "network.h"
#include "connection.h"

void handle_help() {
    printf("help: Hiển thị danh sách tất cả các lệnh và mô tả ngắn gọn cách sử dụng chúng.\n");
    printf("myip: In ra địa chỉ IP của máy đang chạy chương trình.\n");
    printf("myport: In ra cổng (port) mà ứng dụng đang lắng nghe các kết nối đến.\n");
    printf("connect <destination_ip> <port>: Thiết lập một kết nối TCP mới đến một peer khác.\n");
    printf("list: Hiển thị danh sách tất cả các kết nối đang hoạt động.\n");
    printf("terminate <connection_id>: Ngắt một kết nối cụ thể dựa trên ID.\n");
    printf("send <connection_id> <message>: Gửi một tin nhắn đến một peer cụ thể.\n");
    printf("exit: Đóng tất cả các kết nối và thoát khỏi chương trình.\n");
}

void handle_myip() {
    printf("%s\n", my_ip);
}

void handle_myport() {
    printf("%d\n", my_port);
}

void handle_connect(const char *args) {
    char ip[INET_ADDRSTRLEN];
    int port;
    if (sscanf(args, "%15s %d", ip, &port) != 2) {
        printf("Usage: connect <ip> <port>\n");
        return;
    }
    
    if (port < 1 || port > 65535) {
        printf("Invalid port\n");
        return;
    }
    
    struct in_addr addr;
    if (inet_pton(AF_INET, ip, &addr) != 1) {
        printf("Invalid IP\n");
        return;
    }
    
    if (strcmp(ip, my_ip) == 0 && port == my_port) {
        printf("Cannot connect to self\n");
        return;
    }
    
    // Check existing connection
    pthread_mutex_lock(&mutex);
    for (Connection *cur = head; cur; cur = cur->next) {
        if (strcmp(cur->ip, ip) == 0 && cur->port == port) {
            pthread_mutex_unlock(&mutex);
            printf("Connection already exists\n");
            return;
        }
    }
    pthread_mutex_unlock(&mutex);
    
    int sock = connect_to(ip, port);
    if (sock < 0) {
        printf("Failed to connect\n");
        return;
    }
    
    Connection *new_con = (Connection *)malloc(sizeof(Connection));
    if (!new_con) {
        close(sock);
        return;
    }
    
    new_con->id = next_id++;
    new_con->sock = sock;
    strcpy(new_con->ip, ip);
    new_con->port = port;
    pthread_create(&new_con->thread, NULL, recv_thread, new_con);
    add_to_list(new_con);
    printf("Connected to %s:%d as ID %d\n", ip, port, new_con->id);
}

void handle_list() {
    print_connections();
}

void handle_terminate(const char *args) {
    int id;
    if (sscanf(args, "%d", &id) != 1) {
        printf("Usage: terminate <id>\n");
        return;
    }
    
    Connection *c = find_connection(id);
    if (!c) {
        printf("Invalid connection ID\n");
        return;
    }
    
    shutdown(c->sock, SHUT_RDWR);
    close(c->sock);
    pthread_t tid = c->thread;
    
    if (remove_from_list(c)) {
        pthread_join(tid, NULL);
        free(c);
        printf("Connection %d terminated\n", id);
    }
}

void handle_send(const char *args) {
    int id;
    char message[MAX_MESSAGE_LEN + 1];
    
    if (sscanf(args, "%d %100[^\n]", &id, message) != 2) {
        printf("Usage: send <id> <message>\n");
        return;
    }
    
    if (strlen(message) == 0) {
        return;
    }
    
    Connection *c = find_connection(id);
    if (!c) {
        printf("Invalid connection ID\n");
        return;
    }
    
    uint8_t len = (uint8_t)strlen(message);
    if (send(c->sock, &len, 1, 0) < 0 || send(c->sock, message, len, 0) < 0) {
        printf("Failed to send message to connection %d\n", id);
    }
}

void handle_exit() {
    printf("Shutting down...\n");
    running = 0;
    
    // Đóng server socket để listener thread thoát
    if (server_sock != -1) {
        shutdown(server_sock, SHUT_RDWR);
        close(server_sock);
        server_sock = -1;
    }
    
    // Đóng tất cả client connections
    cleanup_connections();
}
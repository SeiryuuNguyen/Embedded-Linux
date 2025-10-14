#include "connection.h"

Connection *head = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int next_id = 1;

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
    
    // Đặt thread thành cancellable để có thể dừng ngay lập tức
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    
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
    
    if (running) {
        printf("Connection ID %d closed.\n", c->id);
        if (remove_from_list(c)) {
            free(c);
        }
    }
    return NULL;
}

void print_connections() {
    pthread_mutex_lock(&mutex);
    printf("ID\tIP\t\tPort\n");
    for (Connection *cur = head; cur; cur = cur->next) {
        printf("%d\t%s\t%d\n", cur->id, cur->ip, cur->port);
    }
    pthread_mutex_unlock(&mutex);
}

void cleanup_connections() {
    pthread_mutex_lock(&mutex);
    Connection *cur = head;
    while (cur) {
        Connection *next = cur->next;
        
        // Hủy thread trước khi đóng socket
        pthread_cancel(cur->thread);
        
        // Đóng socket
        shutdown(cur->sock, SHUT_RDWR);
        close(cur->sock);
        
        // Đợi thread kết thúc
        pthread_join(cur->thread, NULL);
        
        printf("Closed connection %d (%s:%d)\n", cur->id, cur->ip, cur->port);
        free(cur);
        cur = next;
    }
    head = NULL;
    pthread_mutex_unlock(&mutex);
}
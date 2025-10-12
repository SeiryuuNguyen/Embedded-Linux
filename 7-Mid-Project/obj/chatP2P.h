#include <pthread.h>
#include <stdbool.h>

#define MAX_CONNECTIONS 10
#define MAX_CMD_LEN 256
#define MAX_MSG_LEN 100
#define IP_ADDR_LEN 16

// Cấu trúc để lưu trữ thông tin về mỗi kết nối
typedef struct {
    int id;
    int socket_fd;
    char peer_ip;
    int peer_port;
    pthread_t receiver_tid;
    bool is_active;
} connection_t;

// --- Biến toàn cục ---
// Sử dụng 'extern' để thông báo rằng các biến này được định nghĩa ở nơi khác (trong main.c)
extern connection_t connection_list;
extern int connection_count;
extern pthread_mutex_t connection_list_mutex;
extern int g_my_port;
extern volatile bool g_shutdown_flag;

// --- Khai báo hàm (Prototypes) ---

// Từ network.c
int setup_listening_socket();
void handle_connect_command(char* ip_str, char* port_str);
void print_my_ip();

// Từ commands.c
void print_help();
void list_connections();
void handle_send_command(int id, const char* message);
void handle_terminate_command(int id);

// Từ threading.c
void* listener_thread_func(void* arg);
void* receiver_thread_func(void* arg);
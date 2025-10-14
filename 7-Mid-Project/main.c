#include "common.h"
#include "network.h"
#include "connection.h"
#include "commands.h"

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
    
    server_sock = initialize_server(my_port);
    if (server_sock < 0) {
        return 1;
    }
    
    pthread_t listener_th;
    pthread_create(&listener_th, NULL, listener, NULL);
    
    printf("P2P Chat started on %s:%d\n", my_ip, my_port);
    printf("Type 'help' for available commands\n");
    
    char line[1024];
    while (running && fgets(line, sizeof(line), stdin)) {
        char *p = strchr(line, '\n');
        if (p) *p = '\0';
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        
        const char *args = line + strlen(cmd);
        while (*args == ' ') args++; // Skip spaces
        
        if (strcmp(cmd, "help") == 0) {
            handle_help();
        } else if (strcmp(cmd, "myip") == 0) {
            handle_myip();
        } else if (strcmp(cmd, "myport") == 0) {
            handle_myport();
        } else if (strcmp(cmd, "connect") == 0) {
            handle_connect(args);
        } else if (strcmp(cmd, "list") == 0) {
            handle_list();
        } else if (strcmp(cmd, "terminate") == 0) {
            handle_terminate(args);
        } else if (strcmp(cmd, "send") == 0) {
            handle_send(args);
        } else if (strcmp(cmd, "exit") == 0) {
            handle_exit();
            break; // Thoát ngay sau khi xử lý lệnh exit
        } else {
            printf("Unknown command. Use 'help' for list.\n");
        }
    }
    
    // Cleanup final
    if (running) {
        // Nếu thoát bằng cách khác (như Ctrl+C), gọi handle_exit
        handle_exit();
    }
    
    // Đợi listener thread kết thúc
    pthread_join(listener_th, NULL);
    
    printf("P2P Chat stopped\n");
    return 0;
}
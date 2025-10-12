#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CMD_LEN 256

int main()
{
    char cmd_line[MAX_CMD_LEN];
    char *saveptr;

    while(1){
        printf("> ");
        fflush(stdout);

        if(fgets(cmd_line, sizeof(cmd_line), stdin) == NULL){
            break;
        }

        cmd_line[strcspn[cmd_line, "\n"]] = 0;

        char* command = strtok_r(cmd_line, " ", &saveptr);
        if(command == NULL){
            continue;
        }

        if(strcmp(command, "help") == 0){
            printf("***************************** Chat Application ******************************\n");
            printf("Use the commands below:\n");
            printf("%d. %-31s :%s\n", 1, "help", "display user interface options");
            printf("%d. %-31s :%s\n", 2, "myip", "display IP address of this app");
            printf("%d. %-31s :%s\n", 3, "myport", "display listening port of this app");
            printf("%d. %-31s :%s\n", 4, "connect <destination> <port no>", "connect to the app of another computer");
            printf("%d. %-31s :%s\n", 5, "list", "lis all the connections of this app");
            printf("%d. %-31s :%s\n", 6, "terminate <connection id>", "terminate a connection");
            printf("%d. %-31s :%s\n", 7, "send <connection id> <message>", "send a message to a connection");
            printf("%d. %-31s :%s\n", 8, "exit", "close all connection & terminate this app");
        } else if(strcmp(command, "connect") == 0){
            char *ip = strtok_r(NULL, " ", &saveptr);
            char *port_str = strtok_r(NULL, " ", &saveptr);
            //Do something
        } else if(strcmp(command, "exit") == 0){
            break;
        } else{
            printf("Unknow command: %s\n", command);
        }
    }
}
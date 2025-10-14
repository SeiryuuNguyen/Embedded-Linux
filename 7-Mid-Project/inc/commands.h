#ifndef COMMANDS_H
#define COMMANDS_H

#include "common.h"

// Xử lý lệnh người dùng
void handle_help();
void handle_myip();
void handle_myport();
void handle_connect(const char *args);
void handle_list();
void handle_terminate(const char *args);
void handle_send(const char *args);
void handle_exit();

#endif
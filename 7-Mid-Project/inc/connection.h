#ifndef CONNECTION_H
#define CONNECTION_H

#include "common.h"

// Quản lý danh sách kết nối
void add_to_list(Connection *new_con);
Connection *find_connection(int id);
bool remove_from_list(Connection *target);
void cleanup_connections();
void print_connections();

// Thread xử lý nhận tin nhắn
void *recv_thread(void *arg);

#endif
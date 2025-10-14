#ifndef NETWORK_H
#define NETWORK_H

#include "common.h"

// Hàm mạng
void get_local_ip(char *buf);
int connect_to(const char *ip, int port);
int initialize_server(int port);
void *listener(void *arg);

#endif
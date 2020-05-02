#ifndef QUEUES_H
#define QUEUES_H

#include "broker.h"

uint32_t get_id_connection();
uint32_t get_id_message();

void handle_new_connection(uint32_t client_fd);
void handle_reconnect(uint32_t client_fd, reconnect* reconn);

#endif
#ifndef QUEUES_H
#define QUEUES_H

#include "broker.h"
#include "initializer.h"
#include <semaphore.h>

typedef struct t_connection {
    uint32_t socket;
    uint32_t id_connection;
    bool is_connected;
} t_connection;


typedef struct t_receiver {
    t_connection* conn;
    bool sent;
    bool received;
} t_receiver;


typedef struct t_message {
    void* data;
    uint32_t id_message;
    uint32_t id_correlational;
    uint32_t id_queue;
    uint32_t size;
    t_list* receivers;
} t_message;


typedef struct t_message_queue {
    uint32_t id_queue;
    t_queue* messages;
    t_list* subscribers;
    sem_t* sem_message;
    sem_t* sem_all_ack;
} t_message_queue;

t_list* list_queues;
t_list* connections;

uint32_t get_id_connection();
uint32_t get_id_message();

void handle_new_connection(uint32_t client_fd);
void handle_reconnect(uint32_t client_fd, reconnect* reconn);
void handle_subscribe(uint32_t client_fd, subscribe* subs);
void* list_find_with_args(t_list *self, bool(*condition)(void* elem, void* args), void* args);
void add_message_to_queue(void* data, op_code code, uint32_t size_of_data, uint32_t id_message);
void queue_message_sender(void* queue);
void handle_ack(uint32_t client_fd, ack* acknowledgement);

#endif
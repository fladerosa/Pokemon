#ifndef INITIALIZER_H
#define INITIALIZER_H

#include "common_utils.h"
#include "handler.h"
#include "finalizer.h"
#include "dump.h"
#include "queues.h"

typedef struct config_values{
    uint32_t tamano_memoria;
    uint32_t tamano_minimo_particion;
    char* algoritmo_memoria;
    char* algoritmo_reemplazo;
    char* algoritmo_particion_libre;
    char* ip_broker;
    char* puerto_broker;
    uint32_t frecuencia_compactacion;
    char* dump_file;
} config_values;


config_values cfg_values;
uint32_t listening_socket, id_connection, id_message;
on_request p_on_request;
pthread_mutex_t m_id_message, m_id_connection, m_connections;

void initialize();
void fill_config_values();
t_log* create_log_from_config(char* path_key, char* name, char* show_key);
void set_sig_handler(void);
void init_queues();

#endif
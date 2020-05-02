#ifndef INITIALIZER_H
#define INITIALIZER_H

#include "common_utils.h"
#include "handler.h"
#include "finalizer.h"
#include "dump.h"

typedef struct t_message_queue {
    t_queue queue;
    t_list subscribers;
} t_message_queue;

typedef struct t_message {
    
} t_message;

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

typedef struct t_connection {
    uint32_t socket;
    uint32_t id_connection;
} t_connection;

t_list* connections;

config_values cfg_values;
uint32_t listening_socket;
on_request p_on_request;
pthread_mutex_t m_id_message, m_id_connection;
t_message_queue q_new_pokemon, q_appeared_pokemon, q_catch_pokemon,
    q_caught_pokemon, q_get_pokemon, q_localized_pokemon;

void initialize();
void fill_config_values();
t_log* create_log_from_config(char* path_key, char* name, char* show_key);
void set_sig_handler(void);

#endif
#ifndef INITIALIZER_H
#define INITIALIZER_H

#include "common_utils.h"

typedef struct config_values{
    int tamano_memoria;
    int tamano_minimo_particion;
    char* algoritmo_memoria;
    char* algoritmo_reemplazo;
    char* algoritmo_particion_libre;
    char* ip_broker;
    char* puerto_broker;
    int frecuencia_compactacion;
} config_values;

config_values cfg_values;

void initialize();
void fill_config_values();
t_log* create_log_from_config(char* path_key, char* name, char* show_key);

#endif
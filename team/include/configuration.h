#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include<commons/collections/list.h>
#include<commons/string.h>
#include "common_utils.h"


typedef struct position_coach{
    uint32_t id_entrenador;
    char* posix;
    char* posiy;
}position_coach;

typedef struct pokemon_coach{
    uint32_t id_entrenador;
    char * pokemon;   
}pokemon_coach;

typedef struct objective_coach{
    uint32_t id_entrenador;
    char* pokemon;   
}objective_coach;

typedef struct configuration{
       uint32_t nro_team;
       char *ip_team;
       char *puerto_team;
       t_list *posicion_entrenador;
       t_list *pokemon_entrenador;
       t_list *objetivo_entrenador;
       uint32_t tiempo_reconexion; 
       uint32_t  retardo_ciclo_cpu; 
       char *algoritmo_planificacion;
       uint32_t quantum;
       uint32_t estimacion_inicial;
       char *ip_broker;
       char *puerto_broker;       

}configuration;

configuration values;
op_code code;
uint32_t socket_team;
uint32_t socketServer;
on_request request;

void read_config();

void create_obligatory_logger();
void create_optional_logger();

void load_positions_config_team();
void add_position_to_list(char *);
void print_position(char *);
void show(void*);
void load_pokemons_config_team();
void add_pokemon_to_list(char *);
void print_pokemon(char *);
void load_objectives_config_team();
void add_objective_to_list(char *);
void print_objective(char *);
void load_values_config(t_config*);
void destroy_position(position_coach*);
void destroy_pokemon(pokemon_coach*);
void destroy_objective(objective_coach*);
void destroy_lists_and_loaded_elements();

void initialize_team();
void release_resources(); 

enum t_algoritmo {fifo, rr, sjf_sd, sjf_cd};


#endif
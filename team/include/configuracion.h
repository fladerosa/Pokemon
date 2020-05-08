#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include<commons/collections/list.h>
#include<commons/string.h>
#include "common_utils.h"

typedef enum{
	NEW = 0,
	READY,
	EXEC,
	BLOCKED,
	EXIT
} enum_process_state;

typedef struct{
    uint32_t position_x;   
    uint32_t position_y;   
}map;

typedef struct{
    uint32_t id_trainer;
    uint32_t posix;
    uint32_t posiy;
}position_trainer;

typedef struct{
    uint32_t id_trainer;
    position_trainer position;
    enum_process_state state; 
}thread_trainer;

typedef struct{
    uint32_t id_trainer;
    char * pokemon; 
    uint32_t initial_quantity_pokemon;     
}pokemon_trainer;

typedef struct{
    uint32_t id_trainer;
    char* pokemon; 
    uint32_t global_quantity_pokemon;   
}objective_trainer;

typedef struct{
    char* pokemon;
    uint32_t total_init_quantity_pokemon;
    uint32_t total_global_quantity_pokemon;
    uint32_t dif_init_team_quantity_pokemon;    
}pokemon_team;

typedef struct{
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

thread_trainer init_position;
pokemon_trainer init_pokemon;
objective_trainer global_pokemon;
pokemon_team pokemon_to_find;
map map_team;
configuration values;

op_code code;
uint32_t socket_team;
uint32_t socket_broker;
on_request request;

void read_config();
void create_optional_logger();
void create_obligatory_logger();

void load_positions_config_team(t_config*);
void add_position_to_list(uint32_t*);
uint32_t quantity_trainers(t_list*);
void assign_trainer_id(t_list* );
void fix_position(char *);

void load_pokemons_config_team(t_config*);
void add_pokemon_to_list(char *);
void fix_pokemon(char *);

void load_objectives_config_team(t_config*);;
void add_objective_to_list(char *);
void fix_objective(char *);
bool is_repeated_poke(void *);

void destroy_position(position_trainer*);
void destroy_pokemon(pokemon_trainer*);
void destroy_objective(objective_trainer*);

void destroy_lists_and_loaded_elements();

void initialize_team();
void release_resources();

void listen_to_gameboy();
void connection_broker_global_suscribe();
void connection_broker_suscribe_to_appeared_pokemon();
void connection_broker_suscribe_to_caught_pokemon();
void connection_broker_suscribe_to_localized_pokemon();

void reception_message_queue_subscription(uint32_t, uint32_t, uint32_t);
enum t_algoritmo {fifo, rr, sjf_sd, sjf_cd};


#endif
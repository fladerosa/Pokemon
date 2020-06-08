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
    uint32_t **map;  //matriz dinamica para ubicacion de entrenadores y pokemon
    uint32_t *fil;
    uint32_t *col; //tanto el puntero fila como columna de la matriz, permitirar el desplazamiento para la ubicacion
}position_map;

typedef struct{
    uint32_t posix;
    uint32_t posiy;
    
}position_trainer;

typedef struct{
    char * pokemon; 
    uint32_t initial_quantity_pokemon;
}pokemon;

typedef struct{
    uint32_t *init_tot_pokemon;
}t_tot_pokemon;

typedef struct{
    uint32_t initial_dif_species;
}t_dif_pokemon;

typedef struct{
    char* pokemon; 
    uint32_t global_quantity_pokemon;
    uint32_t global_dif_species;
}g_pokemon;

typedef struct{
    uint32_t id_trainer;
    position_trainer *position;
    pokemon *init_pokemon;
    t_tot_pokemon *i_tot_pokemon;
 //   t_dif_pokemon *i_dif_pokemon;
    g_pokemon *global_pokemon; 
    enum_process_state state;
    uint32_t quantity_poke;  //limite a atrapar total de pokes por entrenador, no por tipo
    struct trainer  *next;
}trainer;

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

trainer *init_trainer;
position_trainer position;
pokemon init_pokemon;
t_tot_pokemon i_tot_pokemon;

t_dif_pokemon i_dif_pokemon;

g_pokemon global_pokemon;

pokemon_team pokemon_to_find;
position_map map_team;
configuration values;

op_code code;
uint32_t socket_team;
uint32_t socket_broker;
on_request request;

void read_config();
void create_optional_logger();
void create_obligatory_logger();

void load_positions_config_team(t_config*);
void add_position_to_list(char *);
void fix_position(char *);

void load_pokemons_config_team(t_config*);
void add_pokemon_to_list(char *);
uint32_t* count_pokemon(char*, uint32_t);
t_tot_pokemon* add_count_pokemon_on_memory(uint32_t*);
void fix_pokemon(char *);

void load_objectives_config_team(t_config*);;
void add_objective_to_list(char *);
void read_objectives_to_char(char **);
void fix_objective(char *);

uint32_t quantity_trainers(t_list*);
position_trainer* add_position_trainer_on_memory(t_list*);
pokemon* add_pokemon_trainer_on_memory(t_list*, t_tot_pokemon*);
void assign_data_trainer(t_config*, t_tot_pokemon*, g_pokemon**);

void destroy_position(position_trainer*);
void destroy_pokemon(pokemon*);
void destroy_objective(g_pokemon*);

void destroy_lists_and_loaded_elements();
void free_assign_trainer(trainer *init_trainer);

void initialize_team();
void release_resources();

void listen_to_gameboy();
void retry_on_x_time();
uint32_t caught_default(caught_pokemon*);
void localized_default(localized_pokemon*, pokemon_team*);

void connection_broker_global_suscribe();
void connection_broker_suscribe_to_appeared_pokemon();
void connection_broker_suscribe_to_caught_pokemon();
void connection_broker_suscribe_to_localized_pokemon();

void reception_message_queue_subscription(uint32_t, uint32_t, uint32_t);
enum t_algoritmo {fifo, rr, sjf_sd, sjf_cd};


#endif
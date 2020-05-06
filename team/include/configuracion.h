#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include<commons/collections/list.h>
#include<commons/string.h>
#include "common_utils.h"

#define POSIX 10
#define POSIY 10

//estrucutra para ubicar posiciones de entrenadores y pokemon
typedef struct{
    uint32_t position_trainer_on_map[POSIX][POSIY];   
    uint32_t position_pokemon_on_map[POSIX][POSIY];   
}map;

//estructura para manejar la lista de posiciones del entrenador
typedef struct{
    uint32_t id_trainer;
    uint32_t posix;
    uint32_t posiy;
}position_trainer;

//estructura para manejar la lista de los pokemon que tiene en el inicio por entrenador
typedef struct{
    uint32_t id_trainer;
    char * pokemon; 
    uint32_t initial_quantity_pokemon;     
}pokemon_trainer;

//estructura para manejar la lista de los objetivos pokemon por entrenador
typedef struct{
    uint32_t id_trainer;
    char* pokemon; 
    uint32_t global_quantity_pokemon;   
}objective_trainer;

//estrucutura para el archivo de configuracion
//campos que se agregaron ip y puerto team, logFileOptional y nro de team
typedef struct{
       uint32_t nro_team; //pensando en como identificar n teams
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
position_trainer init_position;
pokemon_trainer init_pokemon;
objetive_trainer global_pokemon;

op_code code;
uint32_t socket_team;
uint32_t socket_broker;
on_request request;

void read_config();

void create_obligatory_logger();
void create_optional_logger();

void load_positions_config_team();
void add_position_to_list(char *);
void fix_position(char *);
void assembly_position_trainer(t_list*);

void load_pokemons_config_team();
void add_pokemon_to_list(char *);
void fix_pokemon(char *);
void count_init_pokemon(t_list*);

void load_objectives_config_team();
void add_objective_to_list(char *);
void fix_objective(char *);
void count_global_pokemon(t_list*);

void load_values_config(t_config*);
void destroy_position(position_coach*);
void destroy_pokemon(pokemon_coach*);
void destroy_objective(objective_coach*);
void destroy_lists_and_loaded_elements();

void initialize_team();
void release_resources();

void listen_to_gameboy();
void establish_broker_connection();

void reception_message_queue_subscription(uint32_t, uint32_t, uint32_t);



enum t_algoritmo {fifo, rr, sjf_sd, sjf_cd};


#endif
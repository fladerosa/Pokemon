#ifndef INICIALIZACION_H
#define INICIALIZACION_H

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
/* Estructura de posición */
typedef struct{
    uint32_t posix;
    uint32_t posiy;   
} t_position_to_map;

typedef struct{
    char *pokemon; 
    t_position_to_map position;
} t_pokemon;

/* Estructura basica de un entrenador*/
typedef struct{
    uint32_t id_trainer;
    t_position_to_map position;
    t_list* pokemonOwned;  //Pokemons that i captured
    t_list* pokemosNeeded; //Pokemons that i must capture to accomplish the objetive
    enum_process_state state;
} t_trainer;

/* Estructura con los datos del archivo de configuración */
typedef struct{
       char *ip_team;
       char *puerto_team;
       //necesito de estas listas para tener todo lo que viene de config
       t_list* posicion_entrenador;
       t_list* pokemon_entrenador;
       t_list* objetivo_entrenador;
       uint32_t tiempo_reconexion; 
       uint32_t  retardo_ciclo_cpu; 
       char *algoritmo_planificacion;
       uint32_t quantum;
       uint32_t estimacion_inicial;
       char *ip_broker;
       char *puerto_broker;       
} t_configuration;

t_trainer *data_trainer;
t_list* trainers; //List of type t_trainer
t_configuration config_values; //Values readed from tema.config

void initialize_team();
void read_config();
void create_optional_logger();
void create_obligatory_logger();

void load_values_config(t_config *);

void load_positions_config_team(t_config*);
void add_position_to_list(char *);
void fix_position(char *);
t_position_to_map* add_position_trainer_on_memory(t_list*);

void load_pokemons_config_team(t_config*);
void add_pokemon_to_list(char *);
void fix_pokemon(char *);


void load_objectives_config_team(t_config*);;
void add_objective_to_list(char *);
void fix_objective(char *);

uint32_t quantity_trainers(t_list*);

void process_trainer(t_config *);
void add_trainer_to_list(t_trainer *);
t_trainer* assign_data_trainer(t_config*);

void destroy_position(t_position_to_map*);
void destroy_pokemon(t_pokemon*);

void destroy_lists_and_loaded_elements();
void free_assign_trainer(t_trainer*);

void release_resources();


#endif

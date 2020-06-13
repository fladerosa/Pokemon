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
} t_position;

typedef struct{
    char *pokemon; 
    t_position position;
} t_pokemon;

/* Estructura basica de un entrenador*/
typedef struct{
    uint32_t id_trainer;
    t_position position;
    t_list* pokemonOwned;  //Pokemons that i captured
    t_list* pokemosNeeded; //Pokemons that i must capture to accomplish the objetive
    enum_process_state state;
} t_trainer;

/* Estructura con los datos del archivo de configuración */
typedef struct{
       char *ip_team;
       char *puerto_team;
       uint32_t tiempo_reconexion; 
       uint32_t  retardo_ciclo_cpu; 
       char *algoritmo_planificacion;
       uint32_t quantum;
       uint32_t estimacion_inicial;
       char *ip_broker;
       char *puerto_broker;       
} t_configuration;

t_list* trainers; //List of type t_trainer
t_configuration config_values; //Values readed from tema.config

void initialize_team();
void read_config();
void create_optional_logger();
void create_obligatory_logger();

void load_positions_config_team(t_config*);
void add_position_to_list(char *);
void fix_position(char *);

void load_pokemons_config_team(t_config*);
void add_pokemon_to_list(char *);
t_tot_pokemon* add_count_pokemon_on_memory(uint32_t*);
t_dif_pokemon* add_dif_count_pokemon_on_memory(uint32_t *);
void fix_pokemon(char *);
void sort_array_pokemon(char*);
uint32_t* count_dif_species(char*);

void load_objectives_config_team(t_config*);;
void add_objective_to_list(char *);
t_gtot_pokemon* add_count_pokemon_objetive_on_memory(uint32_t*);
t_gdif_pokemon* add_count_dif_pokemon_objetive_on_memory(uint32_t*);
void fix_objective(char *);

uint32_t quantity_trainers(t_list*);
position_trainer* add_position_trainer_on_memory(t_list*);
pokemon* add_pokemon_trainer_on_memory(t_list*, t_tot_pokemon*);
g_pokemon* add_global_pokemon_trainer_on_memory(t_list*, t_gtot_pokemon*);
void assign_data_trainer(t_config*, t_tot_pokemon*, t_gtot_pokemon*);

void destroy_position(position_trainer*);
void destroy_pokemon(pokemon*);
void destroy_objective(g_pokemon*);

void destroy_lists_and_loaded_elements();
void free_assign_trainer(trainer *init_trainer);

void release_resources();


#endif

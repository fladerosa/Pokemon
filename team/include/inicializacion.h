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
    t_list* pokemonOwned; ///Pokemons that i captured 
    t_list* pokemonNeeded; //Pokemons that i must capture to accomplish the objetive
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
t_list* pokemonOwnedByTrainer; //List to save pokemonOwned when string_split then add_all on t_trainer
t_list* pokemonNeededByTrainer; //List to save pokemonNeeded when string_split then add all on t_trainer
t_list* pokemon_toCaught;
t_position_to_map position; //struct to save init_position trainer when string_split then assign posix, posiy on t_trainer

void initialize_team();
void read_config();
void create_optional_logger();
void create_obligatory_logger();
void load_values_config(t_config*);
t_list* assign_data_trainer(t_config*);
void add_trainer_to_list(t_list*, t_trainer*);
t_list* calculate_pokemon_to_caught(t_list*, t_list*);
void fix_position(char*);
void fix_pokemonOwned(char*);
void add_to_pokemonOwn_list(char*);
void fix_pokemonNeeded(char*);
void add_to_pokemonNeed_list(char*);
void destroy_position(t_position_to_map*);
void destroy_trainer(t_trainer*);
void destroy_lists_and_loaded_elements();
void release_resources();


#endif




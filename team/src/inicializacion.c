
#include "inicializacion.h"
#include <string.h>

void initialize_team() {    
    read_config();
    create_obligatory_logger();
    create_optional_logger();
    load_values_config(config);
    log_info(optional_logger, "Initialization and configuration upload successful\n", LOG_LEVEL_INFO);    
}

void read_config() {   
    char* config_path = "./cfg/team.config";
    config = config_create(config_path); 
    if(config == NULL) {
        error_show("Error reading TEAM process config on %s\n", config_path);
        exit(CONFIG_FAIL);
    }           
}

t_list* assign_data_trainer(t_config *config) {

trainers = list_create();
t_trainer *data_trainer = malloc(sizeof(data_trainer));    
char** init_position =  config_get_array_value(config, "POSICIONES_ENTRENADORES");
char** pokemonOwns = config_get_array_value(config, "POKEMON_ENTRENADORES");
char** pokemonNeeds = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");

 for(uint32_t i = 0; i< (sizeof init_position/sizeof *init_position); i++) {
     if(data_trainer != NULL) {
         data_trainer[i].id_trainer = i+1;
         data_trainer[i].pokemonOwned = list_create();
         data_trainer[i].pokemonNeeded = list_create();

        string_iterate_lines(init_position, fix_position);
         data_trainer[i].position.posix = position.posix;
         data_trainer[i].position.posiy = position.posiy;

        string_iterate_lines(pokemonOwns, fix_pokemonOwned);
        string_iterate_lines(pokemonNeeds, fix_pokemonNeeded);
        //comparar ambas listas y generar una nueva con los pokemon a capturar
        /*
        t_list* pokemon_toCatch = calculate_pokemon_to_catch(pokemonNeeded, pokemonOwned)
            si pokemonOwned esta vacia -> pokemon_toCatch= pokemonNeeded ok
            no se puede dar el caso de que un entrenador no tenga objetivos a capturar  ok
            recorro la lista de needed y comparo con la de owned añadiendo en 
            pokemon_toCatch  aquellos que no esten en la lista de owned
        */
       t_list* pokemon_toCaught = calculate_pokemon_to_caught(pokemonOwnedByTrainer, pokemonNeededByTrainer);
        
         list_add_all(data_trainer->pokemonOwned, pokemonOwnedByTrainer);
         list_add_all(data_trainer->pokemonNeeded, pokemonNeededByTrainer);
         list_add_all(data_trainer->pokemonNeeded, pokemon_toCaught);
        data_trainer[i].state = NEW;

        add_trainer_to_list(trainers, data_trainer);

         log_info(optional_logger, "Request malloc to TRAINER succesfully\n");   
        }
        else{
         log_info(optional_logger, "Error on request malloc to TRAINER \n");
        }
        list_destroy(pokemonOwnedByTrainer);
        list_destroy(pokemonNeededByTrainer);
        list_destroy(pokemon_toCaught);
 }
      
   return trainers;
}

void add_trainer_to_list(t_list* trainers, t_trainer* data_trainer) {
    if(data_trainer != NULL)
    list_add(trainers, data_trainer);
}

t_list* calculate_pokemon_to_catch(t_list* pokemonOwnedByTrainer, t_list* pokemonNeededByTrainer) {
t_list* pokemon_toCaught = list_create();
    if(list_is_empty(pokemonOwnedByTrainer) == 0) {
        list_add_all(pokemon_toCaught, pokemonNeededByTrainer);
    }



return pokemon_toCaught;
}
void add_to_pokemonOwn_list(char* pokemon) {
pokemonOwnedByTrainer = list_create();
    if(pokemon != NULL)          
         list_add(pokemonOwnedByTrainer,pokemon);
}

void fix_pokemonOwned(char* pokemonArray) {
    if(pokemonArray != NULL) {
        char **pokemonTrainer = string_split(pokemonArray, "|");
        string_iterate_lines(pokemonTrainer, add_to_pokemonOwn_list);
    }
}

void add_to_pokemonNeed_list(char* pokemon) {
 pokemonNeededByTrainer = list_create();
    if(pokemon != NULL)          
         list_add(pokemonNeededByTrainer,pokemon);
}
void fix_pokemonNeeded(char* pokemonArray) {
    if(pokemonArray != NULL) {
        char **pokemonTrainer = string_split(pokemonArray, "|");
        string_iterate_lines(pokemonTrainer, add_to_pokemonNeed_list);
    }
}

void fix_position(char* coordinate) {
t_position_to_map* position = malloc(sizeof(position));
    if(coordinate != NULL) {
        char **position_on_map = string_split(coordinate, "|");
        position->posix = (uint32_t)atoi(position_on_map[0]);
        position->posiy = (uint32_t)atoi(position_on_map[1]);
    }
}

void destroy_position(t_position_to_map* position)
{		
		free(position);
}

void destroy_trainer(t_trainer* trainer)
{   
		free(trainer);
}

void destroy_lists_and_loaded_elements()
{
    
     list_destroy_and_destroy_elements(trainers, (void*)destroy_trainer);
     list_destroy(pokemonOwnedByTrainer);
     list_destroy(pokemonNeededByTrainer);
      destroy_position(&position);
}
void load_values_config(t_config * config) {
    
    config_values.tiempo_reconexion = (uint32_t)config_get_int_value(config, "TIEMPO_RECONEXION");
    config_values.retardo_ciclo_cpu = (uint32_t)config_get_int_value(config, "RETARDO_CICLO_CPU");
    config_values.algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    config_values.quantum = (uint32_t)config_get_int_value(config, "QUANTUM");
    config_values.estimacion_inicial = (uint32_t)config_get_int_value(config, "ESTIMACION_INICIAL");
    config_values.ip_broker = config_get_string_value(config, "IP_BROKER");
    config_values.puerto_broker= config_get_string_value(config, "PUERTO_BROKER");
    config_values.ip_team = config_get_string_value(config, "IP_TEAM");
    config_values.puerto_team = config_get_string_value(config, "PUERTO_TEAM");
}

void create_obligatory_logger() {      
    char* log_obligatory_config = config_get_string_value(config, "LOG_FILE");
    obligatory_logger = log_create(log_obligatory_config, "TEAM", 1, LOG_LEVEL_INFO);
    if(obligatory_logger == NULL) {
        error_show("Error creating TEAM process obligatory logger %s\n", log_obligatory_config);
        exit(LOG_FAIL);
    }
    log_info(obligatory_logger, "Obligatory Log was created successfully\n");
}
 
 void create_optional_logger() {        
    char* log_optional_config = config_get_string_value(config, "LOG_FILE_OPTIONAL");
    optional_logger = log_create(log_optional_config, "TEAM", 1, LOG_LEVEL_INFO);
    if(optional_logger == NULL) {
        error_show("Error creating TEAM process optional logger %s\n", log_optional_config);
        exit(LOG_FAIL);
    }
    log_info(optional_logger, "Optional Log was created successfully\n");
}

void release_resources() { 
    if(config)
        config_destroy(config);

    if(obligatory_logger)
        log_destroy(obligatory_logger);

    if(optional_logger)
        log_destroy(optional_logger);

    destroy_lists_and_loaded_elements();
    
}



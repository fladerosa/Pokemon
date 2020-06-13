
#include "inicializacion.h"
#include <string.h>

void initialize_team() {    
    read_config();
    create_obligatory_logger();
    create_optional_logger();
    load_values_config(config);
    log_info(optional_logger, "Initialization and configuration upload successful\n", LOG_LEVEL_INFO);
    //assign_data_trainer(config, &i_tot_pokemon,&g_tot_pokemon);
}

void read_config() {   
    char* config_path = "./cfg/team.config";
    config = config_create(config_path); 
    if(config == NULL) {
        error_show("Error reading TEAM process config on %s\n", config_path);
        exit(CONFIG_FAIL);
    }           
}

void load_positions_config_team(t_config *config) {      
   // ["1|2", "3|7", "5|5"]
    char** position_config =  config_get_array_value(config, "POSICIONES_ENTRENADORES");
    config_values.posicion_entrenador = list_create(); 
    string_iterate_lines(position_config, fix_position);    
}

void add_position_to_list(char *position) {                                          // "2"     
    if(position != NULL) {
        uint32_t value = (uint32_t)atoi(position);
        list_add(config_values.posicion_entrenador,(void*)value);   /*al finalizar todo la lista queda
                                                               values.posicion_entrenador = {1,2,3,7,5,5}; */
    }  
}

void fix_position(char *value) {
    if(value != NULL) {
      char **positions = string_split(value, "|"); 
            //["1", "2"]
      string_iterate_lines(positions, add_position_to_list);        
    } 
}

t_position_to_map* add_position_trainer_on_memory(t_list* copy_position) {
    
    uint32_t coordinate = 2;
    t_list *coordinates = list_create();
    
    t_position_to_map *position = malloc(sizeof(position));
    if(position != NULL && !list_is_empty(copy_position))
    {
       coordinates = list_take_and_remove(copy_position, coordinate);
       position->posix = (uint32_t)list_remove(coordinates, 1);
       position->posiy = (uint32_t)list_remove(coordinates, 2);
    
       log_info(optional_logger, "Request malloc to POSITION successfully\n");
    }
    else{
       log_info(optional_logger, "Error on request malloc to POSITION \n"); 
    }
    list_destroy(coordinates);
  return position;
}
void load_pokemons_config_team(t_config *config) {    
    // ["Bulbasaur|Pikachu|Bulbasaur", "Charmander|Pikachu", "Pidgey"]
    char** pokemon_config = config_get_array_value(config, "POKEMON_ENTRENADORES");
    config_values.pokemon_entrenador = list_create();
    string_iterate_lines(pokemon_config, fix_pokemon);
    
}

void add_pokemon_to_list(char *pokemon)  {                                      
                                        
    if (pokemon != NULL) 
      list_add(config_values.pokemon_entrenador, (void*)pokemon);    
}

void fix_pokemon(char *value)  {  
     //"Bulbasaur|Pikachu|Bulbasaur"
    if(value != NULL) {     
      char **pokemons = string_split(value, "|");
 // ["Bulbasaur", "Pikachu", "Bulbasaur"]
      string_iterate_lines(pokemons, add_pokemon_to_list);             
    }     
}

void load_objectives_config_team(t_config *config) {   
   char** objective_config = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
   config_values.objetivo_entrenador = list_create();
   string_iterate_lines(objective_config, fix_objective);  
}

void add_objective_to_list(char *objective) {                                         
                                            
    if (objective != NULL)    
      list_add(config_values.objetivo_entrenador,(void*)objective); 
}
  
void fix_objective(char *value) {   

    if(value != NULL) {
      char **objectives = string_split(value, "|");
      string_iterate_lines(objectives, add_objective_to_list);  
    }
}

uint32_t quantity_trainers(t_list* position_trainers) {
  return list_size(position_trainers)/2;
}
//procesar estructura de cada hilo de entrenador
void process_trainer(t_config *config) {

t_trainer *data_trainer = assign_data_trainer(config);
add_trainer_to_list(data_trainer);

}

void add_trainer_to_list(t_trainer *data_trainer) {
   
    trainers = list_create();
     if (data_trainer != NULL)     
      list_add(trainers,(void*)data_trainer);
}

t_trainer * assign_data_trainer(t_config *config) {

   t_list* positions = list_create();
   list_add_all(config_values.posicion_entrenador, positions);

   for(int i = 0; i < quantity_trainers(config_values.posicion_entrenador); i++) {

        t_trainer *data_trainer = malloc(sizeof(data_trainer));    
        if(data_trainer != NULL) {
         data_trainer[i].id_trainer = i+1;
         t_position_to_map *position = add_position_trainer_on_memory(positions); 
         data_trainer[i].position.posix = position[0].posix;
         data_trainer[i].position.posiy = position[1].posiy;
            
            
        }
        else{
         log_info(optional_logger, "Error on request malloc to TRAINER \n");
        }
   } 
   return data_trainer;
}

void destroy_position(t_position_to_map* position)
{		
		free(position);
}

void destroy_pokemon(t_pokemon* pokemon)
{   
		free(pokemon);
}

void free_assign_trainer(t_trainer *trainer)
{
       
    free(trainer);
}

void destroy_lists_and_loaded_elements()
{
     list_destroy_and_destroy_elements(config_values.posicion_entrenador,(void*)destroy_position);
     list_destroy_and_destroy_elements(config_values.pokemon_entrenador, (void*)destroy_pokemon);
     //list_destroy_and_destroy_elements(config_values.objetivo_entrenador, (void*)destroy_objective);
}
void load_values_config(t_config * config) {
    load_positions_config_team(config);
    load_pokemons_config_team(config);
    load_objectives_config_team(config);
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
    //free_assign_trainer(init_trainer); 
}




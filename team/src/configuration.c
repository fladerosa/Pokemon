
#include "configuration.h"

void read_config()
{   
    char* config_path = "./cfg/team.config";
    config = config_create(config_path); 
    if(config == NULL)   
    {
        error_show("Error reading TEAM process config on %s\n", config_path);
        exit(CONFIG_FAIL);
    }           
}
void load_positions_config_team()
{     
    char** position_config =  config_get_array_value(config, "POSICIONES_ENTRENADORES");
    values.posicion_entrenador = list_create();
    string_iterate_lines(position_config, print_position);
    list_iterate(values.posicion_entrenador, show);
}
void add_position_to_list(char *position) {
    if (position != NULL) {
      list_add(values.posicion_entrenador, position);
    }
  }
void print_position(char *value) {
    if(value != NULL) {
      char **positions = string_split(value, "|");
      string_iterate_lines(positions, add_position_to_list);
      printf("Read: %s\n", value);
    } else {
      printf("Empty\n");
    }
  }

void show(void *element) {
    printf("The element: %s\n", (char *)element);
  }

void load_pokemons_config_team()
{
    char** pokemon_config = config_get_array_value(config, "POKEMON_ENTRENADORES");
    values.pokemon_entrenador = list_create();
    string_iterate_lines(pokemon_config, print_pokemon);
    list_iterate(values.pokemon_entrenador, show);

}
void add_pokemon_to_list(char *pokemon) {
    if (pokemon != NULL) {
      list_add(values.pokemon_entrenador, pokemon);
    }
  }
void print_pokemon(char *value) {
    if(value != NULL) {
      char **pokemons = string_split(value, "|");
      string_iterate_lines(pokemons, add_pokemon_to_list);
      printf("Read: %s\n", value);
    } else {
      printf("Empty\n");
    }
  }
void load_objectives_config_team()
{
   char** objective_config = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
   values.objetivo_entrenador = list_create();
   string_iterate_lines(objective_config, print_objective);
   list_iterate(values.objetivo_entrenador, show);
}

void add_objective_to_list(char *objective) {
    if (objective != NULL) {
      list_add(values.objetivo_entrenador, objective);
    }
  }
void print_objective(char *value) {
    if(value != NULL) {
      char **objectives = string_split(value, "|");
      string_iterate_lines(objectives, add_objective_to_list);
      printf("Read: %s\n", value);
    } else {
      printf("Empty: \n");
    }
  }

void destroy_position(position_coach* position){
		
        free(position->posix);
        free(position->posiy);
		free(position);
	}

void destroy_pokemon(pokemon_coach* pokemon){
	
        free(pokemon->pokemon);
		free(pokemon);
	}
void destroy_objective(objective_coach* objective){
	
        free(objective->pokemon);
		free(objective);
	}

void destroy_lists_and_loaded_elements(){

     list_destroy_and_destroy_elements(values.posicion_entrenador,(void*)destroy_position);
     list_destroy_and_destroy_elements(values.pokemon_entrenador, (void*)destroy_pokemon);
     list_destroy_and_destroy_elements(values.objetivo_entrenador, (void*)destroy_objective);
}
void load_values_config(t_config * config)
{
    load_positions_config_team();
    load_pokemons_config_team();
    load_objectives_config_team();
    values.nro_team = (uint32_t)config_get_int_value(config, "NRO_TEAM");
    values.tiempo_reconexion = (uint32_t)config_get_int_value(config, "TIEMPO_RECONEXION");
    values.retardo_ciclo_cpu = (uint32_t)config_get_int_value(config, "RETARDO_CICLO_CPU");
    values.algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    values.quantum = (uint32_t)config_get_int_value(config, "QUANTUM");
    values.estimacion_inicial = (uint32_t)config_get_int_value(config, "ESTIMACION_INICIAL");
    values.ip_broker = config_get_string_value(config, "IP_BROKER");
    values.puerto_broker= config_get_string_value(config, "PUERTO_BROKER");
    values.ip_team = config_get_string_value(config, "IP_TEAM");
    values.puerto_team = config_get_string_value(config, "PUERTO_TEAM");
}

void create_obligatory_logger()
{      
    char* log_obligatory_config = config_get_string_value(config, "LOG_FILE");
    obligatory_logger = log_create(log_obligatory_config, "TEAM", 1, LOG_LEVEL_INFO);
    if(obligatory_logger == NULL)
    {
        error_show("Error creating TEAM process obligatory logger %s\n", log_obligatory_config);
        exit(LOG_FAIL);
    }
    log_info(obligatory_logger, "Obligatory Log was created successfully\n");
}
 
 void create_optional_logger()
{        
    char* log_optional_config = config_get_string_value(config, "LOG_FILE_OPTIONAL");
    optional_logger = log_create(log_optional_config, "TEAM", 1, LOG_LEVEL_INFO);
    if(optional_logger == NULL)
    {
        error_show("Error creating TEAM process optional logger %s\n", log_optional_config);
        exit(LOG_FAIL);
    }
    log_info(optional_logger, "Optional Log was created successfully\n");
}
void initialize_team()
{    
    read_config(config);
    create_obligatory_logger();
    create_optional_logger();
    load_values_config(config);
    log_info(optional_logger, "Initialization and configuration upload successful\n", LOG_LEVEL_INFO);
}

void release_resources()
{ 
   log_info(optional_logger, "Ending Team %d resources\n", values.nro_team);
    if(config)
        config_destroy(config);

    if(obligatory_logger)
        log_destroy(obligatory_logger);

    if(optional_logger)
        log_destroy(optional_logger);

    destroy_lists_and_loaded_elements();
    close(socket_team);  
    close(socketServer);
}

enum t_algoritmo obtenerAlgoritmo()
{
	enum t_algoritmo esAlgoritmo;
	char* sAlgort = values.algoritmo_planificacion;
    if(string_equals_ignore_case(sAlgort, "FIFO"))
		esAlgoritmo = fifo;
	else if(string_equals_ignore_case(sAlgort, "RR"))
		esAlgoritmo = rr;
	else if(string_equals_ignore_case(sAlgort, "SJF-SD"))
		esAlgoritmo = sjf_sd;
	else if(string_equals_ignore_case(sAlgort, "SJF-SD"))
		esAlgoritmo = sjf_cd;
	return esAlgoritmo;
}

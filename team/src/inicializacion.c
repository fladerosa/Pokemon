
#include "inicializacion.h"
#include <string.h>

void initialize_team() {    
    read_config();
    create_obligatory_logger();
    create_optional_logger();
    load_values_config();
    log_info(optional_logger, "Initialization and configuration upload successful\n", LOG_LEVEL_INFO);    
}

void read_config() {   
    char* config_path = "./cfg/team.config";
    config = config_create(config_path); 
    if(config == NULL) {
        error_show("Error creating TEAM process config on %s\t", config_path);
        exit(CONFIG_FAIL);
    }    
    //log_info(optional_logger, "Creation TEAM config path on successfully/n", LOG_LEVEL_INFO);      
}

void assign_data_trainer() {

    trainers = list_create();
    t_trainer *data_trainer;
    char** init_position = config_get_array_value(config, "POSICIONES_ENTRENADORES");
    char** pokemonOwns = config_get_array_value(config, "POKEMON_ENTRENADORES");
    char** pokemonNeeds = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
    char** valorAux;
    char* dataAux;

    for(uint32_t i = 0; init_position[i] != NULL; i++) {
        data_trainer = malloc(sizeof(t_trainer));
        if(data_trainer != NULL) {
            data_trainer->id_trainer = i+1;
            data_trainer->pokemonOwned = list_create();
            data_trainer->pokemonNeeded = list_create();

            valorAux = string_split(init_position[i], "|");
            data_trainer->position.posix = (uint32_t)atoi(valorAux[0]);
            data_trainer->position.posiy = (uint32_t)atoi(valorAux[1]);
            
            valorAux = string_split(pokemonOwns[i], "|");

            while(*valorAux != NULL){
                dataAux = malloc(strlen(*valorAux));
                strcpy(dataAux, *valorAux);
                list_add(data_trainer->pokemonOwned, dataAux);
                valorAux++;
            };

            valorAux = string_split(pokemonNeeds[i], "|");

            while(*valorAux != NULL){
                dataAux = malloc(strlen(*valorAux));
                strcpy(dataAux, *valorAux);
                list_add(data_trainer->pokemonNeeded, dataAux);
                valorAux++;
            };

            data_trainer->state = NEW;

            list_add(trainers, (void*)data_trainer);

            log_info(optional_logger, "Request malloc succesfully to TRAINER %d ", (int)i+1);   
        }else{
            log_info(optional_logger, "Error on request malloc to TRAINER \n");
        }
    }
     
   return;
}

void print_trainer_list() {

	t_list* aux = trainers;

	log_info(optional_logger, "Trainers List: ");
	int i = 0;
	int quantityTrainer = list_size(aux);
	while (i < quantityTrainer) {
		t_trainer* data_trainer;
		data_trainer = (t_trainer*) list_get(aux, i);

		log_info(optional_logger, "Trainer %d: '%c'", i, data_trainer->id_trainer);

		i++;
	}
	if (i == 0)
		log_warning(optional_logger, "Empty list");

	if (aux == NULL)
		log_info(optional_logger, "End list");
}


void destroy_trainer(t_trainer* trainer)
{   
		free(trainer);
}

void destroy_lists_and_loaded_elements()
{   
     list_destroy_and_destroy_elements(trainers, (void*)destroy_trainer);
        
}
void load_values_config() {
    
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



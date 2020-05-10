
#include "configuracion.h"
#include <string.h>

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

void load_positions_config_team(t_config *config)
{      
    char** position_config =  config_get_array_value(config, "POSICIONES_ENTRENADORES");
    values.posicion_entrenador = list_create(); 
    string_iterate_lines(position_config, fix_position);
    assign_trainer_id(values.posicion_entrenador);
    
}

void add_position_to_list(position_trainer *position) 
{
    if (position != NULL) 
    {
        list_add(values.posicion_entrenador,position);
    }  
}

uint32_t quantity_trainers(t_list* position_trainers)
{
  return list_size(position_trainers)/2;
}

void fix_position(char *value) 
{
  position_trainer *position = malloc(sizeof(position_trainer));
    if(value != NULL) 
    {
      char **positions = string_split(value, "|");
      uint32_t value_x = (uint32_t)atoi(positions[0]);
      uint32_t value_y = (uint32_t)atoi(positions[1]);
      position->posix = value_x;
      position->posiy = value_y;

      add_position_to_list(position);     
    }  
}
//procesar estructura de hilo de entrenador
void assign_trainer_id(t_list* position_trainers_on_list)
{
    thread_trainer *init_position_trainer = malloc(sizeof(init_position_trainer));

    for(uint32_t i = 0; i <= quantity_trainers(position_trainers_on_list); i++)
    {     
         init_position_trainer->id_trainer = i+1; 
         init_position_trainer->state = NEW;
    }
}

void load_pokemons_config_team(t_config *config)
{   
    char** pokemon_config = config_get_array_value(config, "POKEMON_ENTRENADORES");
    values.pokemon_entrenador = list_create();
    string_iterate_lines(pokemon_config, fix_pokemon);
    
}

void add_pokemon_to_list(char *pokemon) 
{
    if (pokemon != NULL) 
      list_add(values.pokemon_entrenador, pokemon);    
}

void fix_pokemon(char *value) 
{
    pokemon *init_pokemon = malloc(sizeof(init_pokemon));
    int i = 0;
    uint32_t quantity = 0;

    if(value != NULL) 
    {
      char **pokemons = string_split(value, "|");
      while(pokemons[i] != NULL)
      {
          init_pokemon->pokemon = pokemons[i];
          quantity++;
          
            if((strcmp(pokemons[i],  pokemons[i+1])==0))
            {
                quantity++;
                init_pokemon->initial_quantity_pokemon = quantity;
            }
            else{
                 init_pokemon->initial_quantity_pokemon = quantity;
            }                
        i++;
      }
      string_iterate_lines(pokemons, add_pokemon_to_list);
    } 
}

void load_objectives_config_team(t_config *config)
{
   char** objective_config = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
   values.objetivo_entrenador = list_create();
   string_iterate_lines(objective_config, fix_objective);
   
}

void add_objective_to_list(char *objective) 
{
    if (objective != NULL)    
      list_add(values.objetivo_entrenador, objective); 
}

void fix_objective(char *value) 
{
    g_pokemon *global_pokemon = malloc(sizeof(global_pokemon));
    if(value != NULL) 
    {
      char **objectives = string_split(value, "|");
      int i = 0;
      uint32_t quantity = 0;

      while(objectives[i] != NULL)
      {
          global_pokemon->pokemon = objectives[i];
          quantity++;
          
            if((strcmp(objectives[i],  objectives[i+1])==0))
            {
                quantity++;
                global_pokemon->global_quantity_pokemon = quantity;
            }
            else{
                 global_pokemon->global_quantity_pokemon = quantity;
            }
                
        i++;
      }
      string_iterate_lines(objectives, add_objective_to_list);
    }
}

void destroy_position(thread_trainer* init_position_trainer)
{		
		free(init_position_trainer);
}
/*
void destroy_pokemon(pokemon_trainer* init_pokemon_trainer)
{   
		free(init_pokemon_trainer);
}
void destroy_objective(objective_trainer* global_pokemon_trainer)
{ 
		free(global_pokemon_trainer);
}
*/
void destroy_lists_and_loaded_elements()
{
     list_destroy_and_destroy_elements(values.posicion_entrenador,(void*)destroy_position);
     //list_destroy_and_destroy_elements(values.pokemon_entrenador, (void*)destroy_pokemon);
    // list_destroy_and_destroy_elements(values.objetivo_entrenador, (void*)destroy_objective);
}
void load_values_config(t_config * config)
{
    load_positions_config_team(config);
    load_pokemons_config_team(config);
    load_objectives_config_team(config);
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

    pthread_t connection_broker;
    pthread_create(&connection_broker,NULL,(void*)connection_broker_global_suscribe,NULL); 

    request = &reception_message_queue_subscription; 

    pthread_join(connection_broker,NULL);
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
    close(socket_broker);
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


void listen_to_gameboy()
{ 
   start_server(values.ip_team, values.puerto_team, request);
   pthread_join(server, NULL);
   //mandar mensaje de confirmacion cuando el gameboy me mande appeared_pokemon()
}
void connection_broker_global_suscribe()
{
    connection_broker_suscribe_to_appeared_pokemon();
    connection_broker_suscribe_to_caught_pokemon();
    connection_broker_suscribe_to_localized_pokemon();
 }

void connection_broker_suscribe_to_appeared_pokemon()
{   //se envia un connect por cada cola de mensajes a suscribirse
    connection *connection_server= malloc(sizeof(connection));

    uint32_t server_connection = crear_conexion(values.ip_broker, values.puerto_broker);
    //si la conexion falla(, debe haber un reintento de conexion por el .cfg por medio de un hilo.
    if(server_connection == -1)
    {
        send_reconnect(server_connection); //cada x cantidad de segundos
        log_info(obligatory_logger, "Sending reconnect to broker\n");
    }
    else{
        log_info(obligatory_logger, "Connection to broker succesfully\n");
    }  
    pthread_t connection_broker_appeared_pokemon;
    pthread_create(&connection_broker_appeared_pokemon, NULL, (void*) connection_broker_suscribe_to_appeared_pokemon, NULL);

    connection_server->id_connection = receive_connection_id(server_connection); 
	suscribirseA(APPEARED_POKEMON, server_connection);
		log_info(optional_logger, "Queue subscription request APPEARED_POKEMON\n");

}	
void connection_broker_suscribe_to_caught_pokemon()
{	
    connection *connection_server= malloc(sizeof(connection));
    uint32_t server_connection = crear_conexion(values.ip_broker, values.puerto_broker);
    if(server_connection == -1)
    {
    send_reconnect(server_connection);
    log_info(obligatory_logger, "Sending reconnect to broker\n");
    }
    else{
    log_info(obligatory_logger, "Connection to broker succesfully\n");
    }
    pthread_t connection_broker_caught_pokemon;
    pthread_create(&connection_broker_caught_pokemon, NULL, (void*) connection_broker_suscribe_to_caught_pokemon, NULL);

    connection_server->id_connection = receive_connection_id(server_connection);
    suscribirseA(CAUGHT_POKEMON, server_connection);
		log_info(optional_logger, "Queue subscription request CAUGHT_POKEMON\n");
}

void connection_broker_suscribe_to_localized_pokemon()
{
    connection *connection_server= malloc(sizeof(connection));
    uint32_t server_connection = crear_conexion(values.ip_broker, values.puerto_broker);
    if(server_connection == -1)
    {
    send_reconnect(server_connection);
    log_info(obligatory_logger, "Sending reconnect to broker\n");
    }
    else{
    log_info(obligatory_logger, "Connection to broker succesfully\n");
    }	
    pthread_t connection_broker_localized_pokemon;
    pthread_create(&connection_broker_localized_pokemon, NULL, (void*) connection_broker_suscribe_to_localized_pokemon, NULL);

    connection_server->id_connection = receive_connection_id(server_connection);
	suscribirseA(LOCALIZED_POKEMON, server_connection);
		log_info(optional_logger, "Queue subscription request LOCALIZED_POKEMON\n"); 
    
}

void reception_message_queue_subscription(uint32_t code, uint32_t sizeofstruct, uint32_t client_fd)
{
	void* stream = malloc(sizeofstruct);
    recv(client_fd, stream, sizeofstruct, MSG_WAITALL);

    switch(code){
        case APPEARED_POKEMON:;
            appeared_pokemon* appeared_pokemon_Message = stream_to_appeared_pokemon(stream, NULL, NULL, false); 
            log_info(optional_logger, "Appeared pokemon!");
            break;
        case CATCH_POKEMON:;
            catch_pokemon* catch_Pokemon_Message = stream_to_catch_pokemon(stream, NULL, false);
            log_info(optional_logger, "Catch pokemon!");
            break;
        case CAUGHT_POKEMON:;
			caught_pokemon* caught_Pokemon_Message = stream_to_caught_pokemon(stream, NULL, NULL, false);
            log_info(optional_logger, "Caught pokemon!");
            break;
		case GET_POKEMON:;
            get_pokemon* get_Pokemon_Message = stream_to_get_pokemon(stream, NULL, false);
            log_info(optional_logger, "Get pokemon!"); 
            break;
        case LOCALIZED_POKEMON:;
            localized_pokemon* localized_Pokemon_Message = stream_to_localized_pokemon(stream, NULL, NULL, false);
            log_info(optional_logger, "Localized pokemon!"); 
            break;
    }



}
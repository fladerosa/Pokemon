
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
   // ["1|2", "3|7", "5|5"]
    char** position_config =  config_get_array_value(config, "POSICIONES_ENTRENADORES");
    values.posicion_entrenador = list_create(); 
    string_iterate_lines(position_config, fix_position);    
}

void add_position_to_list(char *position)  // "1"
{                                          // "2"     
    if(position != NULL) 
    {
        uint32_t value = (uint32_t)atoi(position);
        list_add(values.posicion_entrenador,(void*)value);   /*al finalizar todo la lista queda
                                                               values.posicion_entrenador = {1,2,3,7,5,5}; */
    }  
}

void fix_position(char *value) //"1|2"
{
    if(value != NULL) 
    {
      char **positions = string_split(value, "|"); 
            //["1", "2"]
      string_iterate_lines(positions, add_position_to_list);        
    } 
}

void load_pokemons_config_team(t_config *config)
{    // ["Bulbasaur|Pikachu|Bulbasaur", "Charmander|Pikachu", "Pidgey"]
    char** pokemon_config = config_get_array_value(config, "POKEMON_ENTRENADORES");
    values.pokemon_entrenador = list_create();
    string_iterate_lines(pokemon_config, fix_pokemon);
    
}

void add_pokemon_to_list(char *pokemon) // "Bulbasaur" 
{                                       //"Pikachu"
                                        // "Bulbasaur"
    if (pokemon != NULL) 
      list_add(values.pokemon_entrenador, (void*)pokemon);    
}

t_tot_pokemon* add_count_pokemon_on_memory(uint32_t *value)
{
    t_tot_pokemon *i_tot_pokemon = malloc(sizeof(i_tot_pokemon));
        if(i_tot_pokemon != NULL)
            i_tot_pokemon->init_tot_pokemon = value;
        
    return i_tot_pokemon;
}

t_dif_pokemon* add_dif_count_pokemon_on_memory(uint32_t *value)
{
    t_dif_pokemon *i_dif_pokemon = malloc(sizeof(i_dif_pokemon));
        if(i_dif_pokemon != NULL)
            i_dif_pokemon->initial_dif_species = value;
        
    return i_dif_pokemon;
}

void fix_pokemon(char *value) 
{  
    uint32_t *poke =(uint32_t*)0;
    uint32_t *dif_poke =(uint32_t*)0;
     //"Bulbasaur|Pikachu|Bulbasaur"
    if(value != NULL) 
    {     
      char **pokemons = string_split(value, "|");
 // ["Bulbasaur", "Pikachu", "Bulbasaur"]
      string_iterate_lines(pokemons, add_pokemon_to_list);   
            poke++; 
    }  
    t_tot_pokemon *i_tot_pokemon = add_count_pokemon_on_memory(poke);

    dif_poke = count_dif_species(value);
    // ["Bulbasaur", "Bulbasaur", "Pikachu"]
    t_dif_pokemon *i_dif_pokemon = add_dif_count_pokemon_on_memory(dif_poke);  
    
    free(i_tot_pokemon);
    free(i_dif_pokemon);
}

void sort_array_pokemon(char* value)
{
    char*aux;
    for(uint32_t i =0; i<sizeof value/sizeof *value; i++)
    {
        for(uint32_t j=i+1; j<sizeof value/sizeof *value; j++)
        {
            if(strcmp(&value[i], &value[j])>0)
            {
                strcpy(aux, &value[i]);
                strcpy(&value[i], &value[j]);
                strcpy(&value[j], aux);
            }
        }
    }
}

uint32_t* count_dif_species(char*value)
{
    uint32_t poke = 0;
    sort_array_pokemon(value);
    for(uint32_t i =0; i<sizeof value/sizeof *value; i++)
    {
        for(uint32_t j=i+1; j<sizeof value/sizeof *value; j++)
        {
            poke++;
            if(string_equals_ignore_case(&value[i], &value[j])==0)         
                poke++;           
        }
    }
return (uint32_t*)poke;
}

void load_objectives_config_team(t_config *config)
{   // ["Charmander|Pikachu|Bulbasaur", "Squirtle|Pikachu", "Pidgey|Bulbasaur"]
   char** objective_config = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
   values.objetivo_entrenador = list_create();
   string_iterate_lines(objective_config, fix_objective);
   
}

void add_objective_to_list(char *objective) // "Charmander" 
{                                           //"Pikachu"
                                            // "Bulbasaur"
    if (objective != NULL)    
      list_add(values.objetivo_entrenador,(void*)objective); 
}
 
t_gtot_pokemon* add_count_pokemon_objetive_on_memory(uint32_t*value)
{
    t_gtot_pokemon *g_tot_pokemon = malloc(sizeof(g_tot_pokemon));
        
        if(g_tot_pokemon != NULL)
            g_tot_pokemon->global_tot_pokemon = value;
        
    return g_tot_pokemon;
}
t_gdif_pokemon* add_dif_count_pokemon_objetive_on_memory(uint32_t*value)
{
    t_gdif_pokemon *g_dif_pokemon = malloc(sizeof(g_dif_pokemon));
        if(g_dif_pokemon != NULL)
            g_dif_pokemon->global_dif_species = value;
        
        return g_dif_pokemon;
}
 
  
void fix_objective(char *value) 
{   
   uint32_t *poke =(uint32_t*)0; 
   uint32_t *dif_global_poke =(uint32_t*)0;
    //"Charmander|Pikachu|Bulbasaur"
    if(value != NULL) 
    {
      char **objectives = string_split(value, "|");
      string_iterate_lines(objectives, add_objective_to_list);
      poke++;
    }
    t_gtot_pokemon* g_tot_pokemon= add_count_pokemon_objetive_on_memory(poke);
    dif_global_poke = count_dif_species(value);
    t_gdif_pokemon* g_dif_pokemon= add_count_dif_pokemon_objetive_on_memory(dif_global_poke);

    free(g_tot_pokemon);
    free(g_dif_pokemon);
}

uint32_t quantity_trainers(t_list* position_trainers)
{
  return list_size(position_trainers)/2;
}

//procesar estructura de cada hilo de entrenador

position_trainer* add_position_trainer_on_memory(t_list* copy_position)
{
    uint32_t coordinate = 2;
    t_list *coordinates = list_create();
    
    position_trainer *position = malloc(sizeof(position));
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

pokemon* add_pokemon_trainer_on_memory(t_list* copy_pokemon, t_tot_pokemon* i_tot_pokemon)
{
 t_list *pokemones_by_trainer = list_create();
 pokemon *init_pokemon = malloc(sizeof(init_pokemon));

    int pokes = (int)i_tot_pokemon->init_tot_pokemon;
    uint32_t poke = 1;
    if(init_pokemon != NULL && !list_is_empty(copy_pokemon))
    {
        pokemones_by_trainer = list_take_and_remove(copy_pokemon, pokes);

        for(uint32_t j=0; j< pokes; j++)
        {
            init_pokemon->pokemon = list_remove(pokemones_by_trainer, j);
            init_pokemon->initial_quantity_pokemon = poke;
        }
        
      log_info(optional_logger, "Request malloc to POKEMON succesfully\n");   
    }
    else{
       log_info(optional_logger, "Error on request malloc to POKEMON \n"); 
    }

  list_destroy(pokemones_by_trainer);
  return init_pokemon;
}

g_pokemon* add_global_pokemon_trainer_on_memory(t_list* copy_global_pokemon, t_gtot_pokemon* g_tot_pokemon)
{
 t_list *global_pokemones_by_trainer = list_create();
 g_pokemon *global_pokemon = malloc(sizeof(global_pokemon));

    int pokes = (int)g_tot_pokemon->global_tot_pokemon;
    uint32_t poke = 1;
    if(global_pokemon != NULL && !list_is_empty(copy_global_pokemon))
    {
        global_pokemones_by_trainer = list_take_and_remove(copy_global_pokemon, pokes);

        for(uint32_t j=0; j< pokes; j++)
        {
            global_pokemon->pokemon = list_remove(global_pokemones_by_trainer, j);
            global_pokemon->global_quantity_pokemon = poke;
        }
        
      log_info(optional_logger, "Request malloc to GLOBAL POKEMON succesfully\n");   
    }
    else{
       log_info(optional_logger, "Error on request malloc to GLOBAL POKEMON \n"); 
    }

  list_destroy(global_pokemones_by_trainer);
  return global_pokemon;
}

void assign_data_trainer(t_config *config, t_tot_pokemon* i_tot_pokemon, t_gtot_pokemon* g_tot_pokemon)
{
   t_list* trainers = list_create();
   list_add_all(values.posicion_entrenador, trainers);

   t_list* pokemones = list_create();
   list_add_all(values.pokemon_entrenador, pokemones);

   t_list* global_pokemones = list_create();
   list_add_all(values.objetivo_entrenador, pokemones);
    
   for(uint32_t i = 0; i < quantity_trainers(values.posicion_entrenador); i++)
   //while(aux != NULL)
    {
        trainer **init_trainer = malloc(sizeof(init_trainer));    
        if(init_trainer != NULL)
        {
         (*init_trainer+i)->id_trainer = i+1;
         position_trainer *position = add_position_trainer_on_memory(trainers); 
         (*init_trainer+i)->position->posix = position[0].posix;
         (*init_trainer+i)->position->posiy = position[1].posiy;
         
         (*init_trainer+i)->i_tot_pokemon->init_tot_pokemon = i_tot_pokemon->init_tot_pokemon;
         uint32_t max_poke_by_trainer = *i_tot_pokemon->init_tot_pokemon;
         pokemon *init_pokemon = add_pokemon_trainer_on_memory(pokemones, i_tot_pokemon);
        
        for(uint32_t j = 0; j< max_poke_by_trainer; j++)
        {
         strcpy((*init_trainer+i)->init_pokemon->pokemon,(init_pokemon[j]).pokemon);
         (*init_trainer+i)->init_pokemon->initial_quantity_pokemon= (init_pokemon[j]).initial_quantity_pokemon;  
        }

        (*init_trainer+i)->g_tot_pokemon->global_tot_pokemon = g_tot_pokemon->global_tot_pokemon;
         uint32_t max_global_poke_by_trainer = *g_tot_pokemon->global_tot_pokemon;
         g_pokemon *global_pokemon = add_global_pokemon_trainer_on_memory(global_pokemones, g_tot_pokemon);
        
        for(uint32_t k = 0; k< max_global_poke_by_trainer; k++)
        {
         strcpy((*init_trainer+i)->global_pokemon->pokemon, (global_pokemon[k]).pokemon);
         (*init_trainer+i)->global_pokemon->global_quantity_pokemon = (global_pokemon[k]).global_quantity_pokemon;
       
        } 
         
         (*init_trainer+i)->state = NEW;
         log_info(optional_logger, "Request malloc to TRAINER successfully\n");
        }
        else{
         log_info(optional_logger, "Error on request malloc to TRAINER \n");
        }
       // aux = *aux->next;
    }
    list_destroy(trainers);
    list_destroy(pokemones);
    list_destroy(global_pokemones);    
}

void destroy_position(position_trainer* position)
{		
		free(position);
}

void destroy_pokemon(pokemon* init_pokemon)
{   
		free(init_pokemon);
}

void destroy_objective(g_pokemon* global_pokemon)
{ 
		free(global_pokemon);
}

void free_assign_trainer(trainer *init_trainer)
{
        for(uint32_t i = 0; i < quantity_trainers(values.posicion_entrenador); ++i)
        {
            free(init_trainer);
        }
        
    free(init_trainer);
}

void destroy_lists_and_loaded_elements()
{
     list_destroy_and_destroy_elements(values.posicion_entrenador,(void*)destroy_position);
     list_destroy_and_destroy_elements(values.pokemon_entrenador, (void*)destroy_pokemon);
     list_destroy_and_destroy_elements(values.objetivo_entrenador, (void*)destroy_objective);
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

    assign_data_trainer(config, &i_tot_pokemon,&g_tot_pokemon);

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
    free_assign_trainer(init_trainer);
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
   pthread_t listening_gameboy;
   pthread_create(&listening_gameboy, NULL, (void*)listen_to_gameboy, NULL);
   //pthread_join(server, NULL);
   //mandar mensaje de confirmacion cuando el gameboy me mande appeared_pokemon()
}
void connection_broker_global_suscribe()
{
    connection_broker_suscribe_to_appeared_pokemon();
    connection_broker_suscribe_to_caught_pokemon();
    connection_broker_suscribe_to_localized_pokemon();
 }

void retry_on_x_time()
{
    uint32_t time_in = values.tiempo_reconexion;  
    while(time_in != 0)
        time_in--;        
}
uint32_t caught_default(caught_pokemon* caughtPokemonMessage)
{ 
  uint32_t caught_ok = 0;
  caughtPokemonMessage->success =caught_ok;
    return caught_ok;
}

void localized_default(localized_pokemon* localizedPokemonMessage, pokemon_team *pokemon_to_find)
{
  localizedPokemonMessage->pokemon = pokemon_to_find->pokemon;
  t_position* position = list_get(localizedPokemonMessage->positions, 0);

}
void connection_broker_suscribe_to_appeared_pokemon()
{   //se envia un connect por cada cola de mensajes a suscribirse
    connection *connection_server= malloc(sizeof(connection));

    uint32_t server_connection_appeared_pokemon = crear_conexion(values.ip_broker, values.puerto_broker);
    send_new_connection(server_connection_appeared_pokemon); 
    uint32_t id_connection = receive_connection_id(server_connection_appeared_pokemon);

    listen_to_gameboy();
    //si la conexion falla(, debe haber un reintento de conexion por el .cfg por medio de un hilo.
    while(server_connection_appeared_pokemon == -1)
    {
        send_reconnect(server_connection_appeared_pokemon, id_connection);
        pthread_t reconnection_broker_appeared_pokemon;
        pthread_create(&reconnection_broker_appeared_pokemon, NULL, (void*)retry_on_x_time, NULL);
        log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", values.tiempo_reconexion,  reconnection_broker_appeared_pokemon);
    }
    
    log_info(obligatory_logger, "Connection to broker succesfully\n"); 
    pthread_t connection_broker_appeared_pokemon;
    pthread_create(&connection_broker_appeared_pokemon, NULL, (void*)connection_broker_suscribe_to_appeared_pokemon, NULL);

    connection_server->id_connection = receive_connection_id(server_connection_appeared_pokemon); 
	suscribirseA(APPEARED_POKEMON, server_connection_appeared_pokemon);
	log_info(optional_logger, "Queue subscription request APPEARED_POKEMON\n");
}
void connection_broker_suscribe_to_caught_pokemon()
{	
    connection *connection_server= malloc(sizeof(connection));
    uint32_t server_connection_caught_pokemon = crear_conexion(values.ip_broker, values.puerto_broker);

    send_new_connection(server_connection_caught_pokemon); 
    uint32_t id_connection = receive_connection_id(server_connection_caught_pokemon);

    while(server_connection_caught_pokemon == -1)
    {
    send_reconnect(server_connection_caught_pokemon, id_connection);
    pthread_t reconnection_broker_caught_pokemon;
    pthread_create(&reconnection_broker_caught_pokemon, NULL, (void*)retry_on_x_time, NULL);
    log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", values.tiempo_reconexion,  reconnection_broker_caught_pokemon);
    
    caught_pokemon* caughtPokemonMessage =malloc(sizeof(caught_pokemon));
    uint32_t caught=caught_default(caughtPokemonMessage); 
    }
    
    log_info(obligatory_logger, "Connection to broker succesfully\n");
    pthread_t connection_broker_caught_pokemon;
    pthread_create(&connection_broker_caught_pokemon, NULL, (void*) connection_broker_suscribe_to_caught_pokemon, NULL);

    connection_server->id_connection = receive_connection_id(server_connection_caught_pokemon);
    suscribirseA(CAUGHT_POKEMON, server_connection_caught_pokemon);
	log_info(optional_logger, "Queue subscription request CAUGHT_POKEMON\n");
}

void connection_broker_suscribe_to_localized_pokemon()
{
    connection *connection_server= malloc(sizeof(connection));
    uint32_t server_connection_localized_pokemon = crear_conexion(values.ip_broker, values.puerto_broker);
   
    send_new_connection(server_connection_localized_pokemon); 
    uint32_t id_connection = receive_connection_id(server_connection_localized_pokemon);

    while(server_connection_localized_pokemon == -1)
    {
    send_reconnect(server_connection_localized_pokemon, id_connection);
    pthread_t reconnection_broker_localized_pokemon;
    pthread_create(&reconnection_broker_localized_pokemon, NULL, (void*)retry_on_x_time, NULL);
    log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", values.tiempo_reconexion,  reconnection_broker_localized_pokemon);
    
    localized_pokemon* localizedPokemonMessage =malloc(sizeof(localized_pokemon));
    pokemon_team *pokemon_to_find = malloc(sizeof(pokemon_team));
    pokemon_to_find->pokemon = "Bulbasaur";
    localized_default(localizedPokemonMessage, pokemon_to_find); 
    }
    
    log_info(obligatory_logger, "Connection to broker succesfully\n");	
    pthread_t connection_broker_localized_pokemon;
    pthread_create(&connection_broker_localized_pokemon, NULL, (void*) connection_broker_suscribe_to_localized_pokemon, NULL);

    connection_server->id_connection = receive_connection_id(server_connection_localized_pokemon);
	suscribirseA(LOCALIZED_POKEMON, server_connection_localized_pokemon);
	log_info(optional_logger, "Queue subscription request LOCALIZED_POKEMON\n"); 
    
}

void reception_message_queue_subscription(uint32_t code, uint32_t sizeofstruct, uint32_t client_fd)
{
	void* stream = malloc(sizeofstruct);
    uint32_t* id_message = malloc(sizeof(uint32_t));
    recv(client_fd, stream, sizeofstruct, MSG_WAITALL);

    switch(code){
        case APPEARED_POKEMON:;
            appeared_pokemon* appeared_pokemon_Message = stream_to_appeared_pokemon(stream, id_message, NULL, false); 
            log_info(optional_logger, "Appeared pokemon!");
            break;
        case CATCH_POKEMON:;
            catch_pokemon* catch_Pokemon_Message = stream_to_catch_pokemon(stream, id_message, false);
            log_info(optional_logger, "Catch pokemon!");
            break;
        case CAUGHT_POKEMON:;
			caught_pokemon* caught_Pokemon_Message = stream_to_caught_pokemon(stream, id_message, NULL, false);
            log_info(optional_logger, "Caught pokemon!");
            break;
		case GET_POKEMON:;
            get_pokemon* get_Pokemon_Message = stream_to_get_pokemon(stream, id_message, false);
            log_info(optional_logger, "Get pokemon!"); 
            break;
        case LOCALIZED_POKEMON:;
            localized_pokemon* localized_Pokemon_Message = stream_to_localized_pokemon(stream, id_message, NULL, false);
            log_info(optional_logger, "Localized pokemon!"); 
            break;
    }



}
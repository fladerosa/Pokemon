#include "suscripcion.h"
#include <string.h>


void listen_to_gameboy(t_configuration *config_values) { 
   start_server(config_values->ip_team, config_values->puerto_team, request);
   pthread_t listening_gameboy;
   pthread_create(&listening_gameboy, NULL, (void*)listen_to_gameboy, NULL);
   //pthread_join(server, NULL);
   //mandar mensaje de confirmacion cuando el gameboy me mande appeared_pokemon()
}
void connection_broker_global_suscribe() {
    connection_broker_suscribe_to_appeared_pokemon();
    connection_broker_suscribe_to_caught_pokemon();
    connection_broker_suscribe_to_localized_pokemon();
 }

void retry_on_x_time() {
    uint32_t time_in = config_values.tiempo_reconexion;  
    while(time_in != 0)
        time_in--;        
}
uint32_t caught_default(caught_pokemon* caughtPokemonMessage) { 
  uint32_t caught_ok = 0;
  caughtPokemonMessage->success =caught_ok;
    return caught_ok;
}

void localized_default(localized_pokemon* localizedPokemonMessage, pokemon_team *pokemon_to_find) {
  localizedPokemonMessage->pokemon = pokemon_to_find->pokemon;
  t_position* position = list_get(localizedPokemonMessage->positions, 0);

}
void connection_broker_suscribe_to_appeared_pokemon(t_configuration *config_values) {   
    //se envia un connect por cada cola de mensajes a suscribirse
    connection *connection_server= malloc(sizeof(connection));

    uint32_t server_connection_appeared_pokemon = crear_conexion(config_values->ip_broker, config_values->puerto_broker);
    send_new_connection(server_connection_appeared_pokemon); 
    uint32_t id_connection = receive_connection_id(server_connection_appeared_pokemon);

    listen_to_gameboy(config_values);
    //si la conexion falla(, debe haber un reintento de conexion por el .cfg por medio de un hilo.
    while(server_connection_appeared_pokemon == -1) {
        send_reconnect(server_connection_appeared_pokemon, id_connection);
        pthread_t reconnection_broker_appeared_pokemon;
        pthread_create(&reconnection_broker_appeared_pokemon, NULL, (void*)retry_on_x_time, NULL);
        log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values->tiempo_reconexion,  reconnection_broker_appeared_pokemon);
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
    uint32_t server_connection_caught_pokemon = crear_conexion(config_values.ip_broker, config_values.puerto_broker);

    send_new_connection(server_connection_caught_pokemon); 
    uint32_t id_connection = receive_connection_id(server_connection_caught_pokemon);

    while(server_connection_caught_pokemon == -1) {
    send_reconnect(server_connection_caught_pokemon, id_connection);
    pthread_t reconnection_broker_caught_pokemon;
    pthread_create(&reconnection_broker_caught_pokemon, NULL, (void*)retry_on_x_time, NULL);
    log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_caught_pokemon);
    
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

void connection_broker_suscribe_to_localized_pokemon() {
    connection *connection_server= malloc(sizeof(connection));
    uint32_t server_connection_localized_pokemon = crear_conexion(config_values.ip_broker, config_values.puerto_broker);
   
    send_new_connection(server_connection_localized_pokemon); 
    uint32_t id_connection = receive_connection_id(server_connection_localized_pokemon);

    while(server_connection_localized_pokemon == -1) {
    send_reconnect(server_connection_localized_pokemon, id_connection);
    pthread_t reconnection_broker_localized_pokemon;
    pthread_create(&reconnection_broker_localized_pokemon, NULL, (void*)retry_on_x_time, NULL);
    log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_localized_pokemon);
    
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

void reception_message_queue_subscription(uint32_t code, uint32_t sizeofstruct, uint32_t client_fd) {
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
void close_socket() {
    close(socket_team);  
    close(socket_broker);
}
void new_thread_connection_broker_global_suscribe() {
    pthread_t connection_broker;
    pthread_create(&connection_broker,NULL,(void*)connection_broker_global_suscribe,NULL); 
    request = &reception_message_queue_subscription; 
    pthread_join(connection_broker,NULL);
}
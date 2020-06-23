#include "suscripcion.h"
#include <string.h>

void connection_broker_global_suscribe() {
    connection_broker_suscribe_to_appeared_pokemon(APPEARED_POKEMON, &suscripcionAppearedPokemon);
    connection_broker_suscribe_to_caught_pokemon(CAUGHT_POKEMON, &suscripcionCaughtPokemon);
    connection_broker_suscribe_to_localized_pokemon(LOCALIZED_POKEMON, &suscripcionLocalizedPokemon);
 }

void connection_broker_suscribe_to_appeared_pokemon(op_code code, pthread_t* threadAppearedPokemon) {   
    //se envia un connect por cada cola de mensajes a suscribirse
    args_pthread* arguments;
    uint32_t server_connection_appeared_pokemon = crear_conexion(config_values->ip_broker, config_values->puerto_broker);
    send_new_connection(server_connection_appeared_pokemon); 

    pthread_mutex_lock(&reconnectAppearedPokemon);
    //si la conexion falla(, debe haber un reintento de conexion por el .cfg por medio de un hilo.
    while(server_connection_appeared_pokemon == -1) {

        pthread_mutex_unlock(&reconnectAppearedPokemon);

        send_reconnect(server_connection_appeared_pokemon, id_connection);
        pthread_t reconnection_broker_appeared_pokemon;
        pthread_create(&reconnection_broker_appeared_pokemon, NULL, (void*)retry_on_x_time, NULL);
        log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values->tiempo_reconexion,  reconnection_broker_appeared_pokemon);
    }
    
    log_info(obligatory_logger, "Connection to broker succesfully\n"); 
    arguments = thread_suscribe_arguments(code, server_connection_appeared_pokemon);    
    pthread_create(&threadAppearedPokemon, NULL, (void*)suscribeOnThreadList, arguments);
	
}

void connection_broker_suscribe_to_caught_pokemon(op_code code, pthread_t* threadCaughtPokemon)
{	
    uint32_t server_connection_caught_pokemon = crear_conexion(config_values.ip_broker, config_values.puerto_broker);
    send_new_connection(server_connection_caught_pokemon); 

    pthread_mutex_lock(&reconnectCaughtPokemon);

    while(server_connection_caught_pokemon == -1) {
    
    pthread_mutex_unlock(&reconnectCaughtPokemon);
    send_reconnect(server_connection_caught_pokemon, id_connection);
    pthread_t reconnection_broker_caught_pokemon;
    pthread_create(&reconnection_broker_caught_pokemon, NULL, (void*)retry_on_x_time, NULL);
    log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_caught_pokemon);
  
    }    
    log_info(obligatory_logger, "Connection to broker succesfully\n");
    arguments = thread_suscribe_arguments(code, server_connection_caught_pokemon);   
    pthread_create(&threadCaughtPokemon, NULL, (void*)suscribeOnThreadList, arguments);
	
}

void connection_broker_suscribe_to_localized_pokemon(op_code code, pthread_t* threadLocalizedPokemon) {
    
    uint32_t server_connection_localized_pokemon = crear_conexion(config_values.ip_broker, config_values.puerto_broker);   
    send_new_connection(server_connection_localized_pokemon); 

    pthread_mutex_lock(&reconnectLocalizedPokemon);
   
    while(server_connection_localized_pokemon == -1) {

    pthread_mutex_unlock(&reconnectLocalizedPokemon);
    send_reconnect(server_connection_localized_pokemon, id_connection);
    pthread_t reconnection_broker_localized_pokemon;
    pthread_create(&reconnection_broker_localized_pokemon, NULL, (void*)retry_on_x_time, NULL);
    log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_localized_pokemon);
     
    }
    
    log_info(obligatory_logger, "Connection to broker succesfully\n");	
    arguments = thread_suscribe_arguments(code, server_connection_caught_pokemon);   
    pthread_create(&threadLocalizedPokemon, NULL, (void*)suscribeOnThreadList, arguments); 	
    
}
args_pthread* thread_suscribe_arguments(op_code code, uint32_t socket) {

    args_pthread* arguments = malloc(sizeof(args_pthread)); 
    arguments->codigoCola = code;
    arguments->socket = malloc(sizeof(uint32_t));
    *arguments->socket = socket;
return arguments;
}

void suscribeOnThreadList(args_pthread* arguments){
    suscribirseA(arguments->codigoCola,*arguments->socket);
    structAppearedPokemon = malloc(sizeof(threadSubscribe));
    structCaughtPokemon = malloc(sizeof(threadSubscribe));
    structLocalizedPokemon = malloc(sizeof(threadSubscribe));
    switch(arguments->codigoCola){
        case APPEARED_POKEMON:
            structAppeareadPokemon->idCola = APPEARED_POKEMON;
            structAppearedPokemon->socket = *arguments->socket;
            list_add(threadSubscribeList, structAppearedPokemon);
            log_info(optional_logger, "Queue subscription request APPEARED_POKEMON successfully.\n");
            break;
        case CAUGHT_POKEMON:
            structCaughtPokemon->idCola = CAUGHT_POKEMON;
            structCaughtPokemon->socket = *arguments->socket;
            list_add(threadSubscribeList, structCaughtPokemon);
            log_info(optional_logger, "Queue subscription request CAUGHT_POKEMON successfully.\n");
            break;
        case LOCALIZED_POKEMON:
            structLocalizedPokemon->idCola = LOCALIZED_POKEMON;
            structLocalizedPokemon->socket = *arguments->socket;
            list_add(threadSubscribeList, structLocalizedPokemon);
            log_info(optional_logger, "Queue subscription request LOCALIZED_POKEMON successfully.\n");
            break;
        default:
            break;
    }
    
}

void listen_to_gameboy() { 
   start_server(config_values->ip_team, config_values->puerto_team, request);

   pthread_create(&listening_gameboy, NULL, (void*)listen_to_gameboy, NULL);
   //pthread_join(server, NULL);
   //mandar mensaje de confirmacion cuando el gameboy me mande appeared_pokemon()
}

void reception_message_queue_subscription(uint32_t code, uint32_t sizeofstruct, uint32_t client_fd) {
	void* stream = malloc(sizeofstruct);
    uint32_t* id_message = malloc(sizeof(uint32_t));
    recv(client_fd, stream, sizeofstruct, MSG_WAITALL);

    switch(code){
        case APPEARED_POKEMON:;
            appeared_pokemon* appeared_pokemon_Message = stream_to_appeared_pokemon(stream, id_message, NULL, false); 
            log_info(optional_logger, "Receiving Message Appeared pokemon.");
            send_ack(client_fd, *id_message);
            break;
        case CATCH_POKEMON:;
            catch_pokemon* catch_Pokemon_Message = stream_to_catch_pokemon(stream, id_message, false);
            log_info(optional_logger, "Receiving Message Catch pokemon.");
            send_ack(client_fd, *id_message);
            break;
        case CAUGHT_POKEMON:;
			caught_pokemon* caught_Pokemon_Message = stream_to_caught_pokemon(stream, id_message, NULL, false);
            if(caught_Pokemon_Message == NULL) {
                caught_Pokemon_Message = caught_default();
                log_info(optional_logger, "Using Caught pokemon default.");
            }
            log_info(optional_logger, "Receiving Message Caught pokemon.");
            send_ack(client_fd, *id_message);
            break;
		case GET_POKEMON:;
            get_pokemon* get_Pokemon_Message = stream_to_get_pokemon(stream, id_message, false);
            log_info(optional_logger, "Receiving Message Get pokemon."); 
            send_ack(client_fd, *id_message);
            break;
        case LOCALIZED_POKEMON:;
            localized_pokemon* localized_Pokemon_Message = stream_to_localized_pokemon(stream, id_message, NULL, false);
            if(localized_Pokemon_Message == NULL) {
                localized_Pokemon_Message = localized_default();
                log_info(optional_logger, "Using Localized pokemon default.");
            }
            
            log_info(optional_logger, "Receiving Message Localized pokemon.");
            send_ack(client_fd, *id_message); 
            break;
         case CONNECTION:;
            connection* connectionMessage = stream_to_connection(stream);

            threadSubscribe* thread = list_find_with_args(threadSubscribeList, compareSockets, (void*)client_fd);
            thread->idConnection = connectionMessage->id_connection;

            suscribirseA(thread->idQueue, client_fd);

            log_info(optional_logger, "Established Connection."); 
            log_info(optional_logger, "This is the id connection: %d", connectionMessage->id_connection);
            log_info(optional_logger, "Subscribing to queues %d, %d and & %d", APPEARED_POKEMON, CAUGHT_POKEMON, LOCALIZED_POKEMON);
            break;
    }

}
bool compareSockets(void* element, void* args){
    
    threadSubscribe* thread = (threadSubscribe*) element;
    
    return thread->socket == (uint32_t) args;
}
void close_sockets() {
    close(socket_team);  
    close(socket_broker);
}

void retry_on_x_time() {
    uint32_t time_in = config_values.tiempo_reconexion;  
    while(time_in != 0)
        time_in--;        
}

caught_pokemon* caught_default() { 

    bool success = 0;
    return init_caught_pokemon(success);
}

void localized_default(char* pokemon) {
 t_list* positions = list_create();
 localized_pokemon* loca=  init_localized_pokemon(pokemon, t_list* positions);
 positions = list_size(positions);
return  list_size(positions) == 0;

}
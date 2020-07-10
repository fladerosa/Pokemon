#include "suscripcion.h"
#include <string.h>

void connection_broker_global_suscribe() {
    connection_broker_suscribe_to_appeared_pokemon(APPEARED_POKEMON, suscripcionAppearedPokemon);
    connection_broker_suscribe_to_caught_pokemon(CAUGHT_POKEMON, suscripcionCaughtPokemon);
    connection_broker_suscribe_to_localized_pokemon(LOCALIZED_POKEMON, suscripcionLocalizedPokemon);
 }

void connection_broker_suscribe_to_appeared_pokemon(op_code code, pthread_t suscripcionAppearedPokemon) {   
    //se envia un connect por cada cola de mensajes a suscribirse
    uint32_t server_connection_appeared_pokemon = crear_conexion(config_values.ip_broker, config_values.puerto_broker);
    send_new_connection(server_connection_appeared_pokemon); 

    pthread_mutex_lock(&reconnectAppearedPokemon);
    //si la conexion falla(, debe haber un reintento de conexion por el .cfg por medio de un hilo.
    while(server_connection_appeared_pokemon == -1) {

        pthread_mutex_unlock(&reconnectAppearedPokemon);
        reconnect* reconnectMessage = malloc(sizeof(reconnect));
        send_reconnect(server_connection_appeared_pokemon, reconnectMessage->id_connection);
        pthread_t reconnection_broker_appeared_pokemon;
        pthread_create(&reconnection_broker_appeared_pokemon, NULL, (void*)retry_on_x_time, NULL);
        log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_appeared_pokemon);
    }
    
    log_info(obligatory_logger, "Connection to broker succesfully\n"); 
    args_pthread *arguments = thread_suscribe_arguments(code, server_connection_appeared_pokemon);    
    pthread_create(&suscripcionAppearedPokemon, NULL, (void*)suscribeOnThreadList, arguments);
	
}

void connection_broker_suscribe_to_caught_pokemon(op_code code, pthread_t suscripcionCaughtPokemon)
{	
    uint32_t server_connection_caught_pokemon = crear_conexion(config_values.ip_broker, config_values.puerto_broker);
    send_new_connection(server_connection_caught_pokemon); 

    pthread_mutex_lock(&reconnectCaughtPokemon);

    while(server_connection_caught_pokemon == -1) {
    
    pthread_mutex_unlock(&reconnectCaughtPokemon);
    reconnect* reconnectMessage = malloc(sizeof(reconnect));
    send_reconnect(server_connection_caught_pokemon, reconnectMessage->id_connection);
    pthread_t reconnection_broker_caught_pokemon;
    pthread_create(&reconnection_broker_caught_pokemon, NULL, (void*)retry_on_x_time, NULL);
    log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_caught_pokemon);
  
    }    
    log_info(obligatory_logger, "Connection to broker succesfully\n");
    args_pthread *arguments = thread_suscribe_arguments(code, server_connection_caught_pokemon);   
    pthread_create(&suscripcionCaughtPokemon, NULL, (void*)suscribeOnThreadList, arguments);
	
}

void connection_broker_suscribe_to_localized_pokemon(op_code code, pthread_t suscripcionLocalizedPokemon) {
    
    uint32_t server_connection_localized_pokemon = crear_conexion(config_values.ip_broker, config_values.puerto_broker);   
    send_new_connection(server_connection_localized_pokemon); 

    pthread_mutex_lock(&reconnectLocalizedPokemon);
   
    while(server_connection_localized_pokemon == -1) {

    pthread_mutex_unlock(&reconnectLocalizedPokemon);
    reconnect* reconnectMessage = malloc(sizeof(reconnect));
    send_reconnect(server_connection_localized_pokemon, reconnectMessage->id_connection);
    pthread_t reconnection_broker_localized_pokemon;
    pthread_create(&reconnection_broker_localized_pokemon, NULL, (void*)retry_on_x_time, NULL);
    log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_localized_pokemon);
     
    }
    
    log_info(obligatory_logger, "Connection to broker succesfully\n");	
    args_pthread *arguments = thread_suscribe_arguments(code, server_connection_localized_pokemon);   
    pthread_create(&suscripcionLocalizedPokemon, NULL, (void*)suscribeOnThreadList, arguments); 	
    
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
            structAppearedPokemon->idQueue = APPEARED_POKEMON;
            structAppearedPokemon->socket = *arguments->socket;
            list_add(threadSubscribeList, structAppearedPokemon);
            log_info(optional_logger, "Queue subscription request APPEARED_POKEMON successfully.\n");
            break;
        case CAUGHT_POKEMON:
            structCaughtPokemon->idQueue = CAUGHT_POKEMON;
            structCaughtPokemon->socket = *arguments->socket;
            list_add(threadSubscribeList, structCaughtPokemon);
            log_info(optional_logger, "Queue subscription request CAUGHT_POKEMON successfully.\n");
            break;
        case LOCALIZED_POKEMON:
            structLocalizedPokemon->idQueue = LOCALIZED_POKEMON;
            structLocalizedPokemon->socket = *arguments->socket;
            list_add(threadSubscribeList, structLocalizedPokemon);
            log_info(optional_logger, "Queue subscription request LOCALIZED_POKEMON successfully.\n");
            break;
        default:
            break;
    }
    
}

void listen_to_gameboy() { 
   start_server(config_values.ip_team, config_values.puerto_team, request);

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
            log_info(optional_logger, "Receiving Message Appeared pokemon %s in position %d.", appeared_pokemon_Message->pokemon, appeared_pokemon_Message->position); 
            send_ack(client_fd, *id_message);
            break;
        case CATCH_POKEMON:;
            catch_pokemon* catch_Pokemon_Message = stream_to_catch_pokemon(stream, id_message, false);
            log_info(optional_logger, "Receiving Message Catch pokemon %s in position %d.", catch_Pokemon_Message->pokemon, catch_Pokemon_Message->position);
            send_ack(client_fd, *id_message);
            break;
        case CAUGHT_POKEMON:;
			caught_pokemon* caught_Pokemon_Message = stream_to_caught_pokemon(stream, id_message, NULL, false);
            if(caught_Pokemon_Message == NULL) {
                caught_Pokemon_Message = caught_default();
                log_info(optional_logger, "Using Caught pokemon default.");
            }
            log_info(optional_logger, "Receiving Message Caught pokemon with result %d.", caught_Pokemon_Message->success);
            send_ack(client_fd, *id_message);
            break;
		case GET_POKEMON:;
            get_pokemon* get_Pokemon_Message = stream_to_get_pokemon(stream, id_message, false);
            log_info(optional_logger, "Receiving Message Get pokemon %s with quantity %d.", get_Pokemon_Message->pokemon, get_Pokemon_Message->sizePokemon); 
            send_ack(client_fd, *id_message);
            break;
        case LOCALIZED_POKEMON:;
            localized_pokemon* localized_Pokemon_Message = stream_to_localized_pokemon(stream, id_message, NULL, false);
          /*  if(localized_Pokemon_Message == NULL) {
                localized_Pokemon_Message = localized_default();
                log_info(optional_logger, "Using Localized pokemon default.");
            }
            */
            log_info(optional_logger, "Receiving Message Localized pokemon %s with position %d.", localized_Pokemon_Message->pokemon, localized_Pokemon_Message->positions);
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

uint32_t localized_default(char* pokemon) {
 t_list* positions = list_create();
 localized_pokemon* loca=  init_localized_pokemon(pokemon, positions);
 loca->pokemon = pokemon;
 uint32_t position_default = list_size(loca->positions);
return  position_default;

}

//envio de mensaje en el inicio, cola get y catch
//OJO ES UN GET POR CADA ESPECIE NECESARIA PARA CUMPLIR EL OBJETIVO DEL TEAM
void send_message_queue_init(uint32_t code, uint32_t sizeofstruct, uint32_t client_fd) {
    void* stream;
    uint32_t* id_message = malloc(sizeof(uint32_t));
    send(client_fd, stream, sizeofstruct, MSG_WAITALL);

    switch(code) {
        case GET_POKEMON:;
        get_pokemon* getPokemonMessage = malloc(sizeof(get_pokemon));
        stream =get_pokemon_to_stream(getPokemonMessage, id_message);
        log_info(optional_logger, "Sending Message Get pokemon.");
            send_ack(client_fd, *id_message);
        break;

        case CATCH_POKEMON:;
        catch_pokemon* catchPokemonMessage = malloc(sizeof(catch_pokemon));
        stream =catch_pokemon_to_stream(catchPokemonMessage, id_message);
        log_info(optional_logger, "Sending Message Catch pokemon.");
            send_ack(client_fd, *id_message);
        break;

        default:
        break;
    }

}
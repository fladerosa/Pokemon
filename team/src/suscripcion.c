#include "suscripcion.h"
#include <string.h>
#include "team.h"

void* connection_broker_global_suscribe() {
    threadSubscribeList = list_create();
    connection_broker_suscribe_to_appeared_pokemon(APPEARED_POKEMON);
    connection_broker_suscribe_to_caught_pokemon(CAUGHT_POKEMON);
    connection_broker_suscribe_to_localized_pokemon(LOCALIZED_POKEMON);
    
    pthread_join(suscripcionAppearedPokemon,NULL);
    pthread_join(suscripcionCaughtPokemon,NULL);
    pthread_join(suscripcionLocalizedPokemon,NULL);

    return NULL;
 }

void connection_broker_suscribe_to_appeared_pokemon(op_code code) {   
    //se envia un connect por cada cola de mensajes a suscribirse
    uint32_t server_connection_appeared_pokemon = crear_conexion(config_values.ip_broker, config_values.puerto_broker);
    send_new_connection(server_connection_appeared_pokemon); 

    //si la conexion falla(, debe haber un reintento de conexion por el .cfg por medio de un hilo.
    /*
    while(server_connection_appeared_pokemon == -1) {
        reconnect* reconnectMessage = malloc(sizeof(reconnect));
        send_reconnect(server_connection_appeared_pokemon, reconnectMessage->id_connection);
        pthread_t reconnection_broker_appeared_pokemon;
        pthread_create(&reconnection_broker_appeared_pokemon, NULL, (void*)retry_on_x_time, NULL);
        log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_appeared_pokemon);
    }*/
    
    log_info(obligatory_logger, "Connection to broker succesfully\n"); 
    args_pthread *arguments = thread_suscribe_arguments(code, server_connection_appeared_pokemon);    
    pthread_create(&suscripcionAppearedPokemon, NULL, (void*)suscribeOnThreadList, arguments);
	
}

void connection_broker_suscribe_to_caught_pokemon(op_code code) {	
    uint32_t server_connection_caught_pokemon = crear_conexion(config_values.ip_broker, config_values.puerto_broker);
    send_new_connection(server_connection_caught_pokemon); 

/*
    while(server_connection_caught_pokemon == -1) {
    
        reconnect* reconnectMessage = malloc(sizeof(reconnect));
        send_reconnect(server_connection_caught_pokemon, reconnectMessage->id_connection);
        pthread_t reconnection_broker_caught_pokemon;
        pthread_create(&reconnection_broker_caught_pokemon, NULL, (void*)retry_on_x_time, NULL);
        log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_caught_pokemon);
  
    }    */
    log_info(obligatory_logger, "Connection to broker succesfully\n");
    args_pthread *arguments = thread_suscribe_arguments(code, server_connection_caught_pokemon);   
    pthread_create(&suscripcionCaughtPokemon, NULL, (void*)suscribeOnThreadList, arguments);
	
}

void connection_broker_suscribe_to_localized_pokemon(op_code code) {
    uint32_t server_connection_localized_pokemon = crear_conexion(config_values.ip_broker, config_values.puerto_broker);   
    send_new_connection(server_connection_localized_pokemon); 

   /*
    while(server_connection_localized_pokemon == -1) {
        reconnect* reconnectMessage = malloc(sizeof(reconnect));
        send_reconnect(server_connection_localized_pokemon, reconnectMessage->id_connection);
        pthread_t reconnection_broker_localized_pokemon;
        pthread_create(&reconnection_broker_localized_pokemon, NULL, (void*)retry_on_x_time, NULL);
        log_info(obligatory_logger, "Sending reconnect to broker each %d on thread %ul\n", config_values.tiempo_reconexion,  reconnection_broker_localized_pokemon);
    }*/
    
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
    
    switch(arguments->codigoCola){
        case APPEARED_POKEMON:
            structAppearedPokemon = malloc(sizeof(threadSubscribe));
            structAppearedPokemon->idQueue = APPEARED_POKEMON;
            structAppearedPokemon->socket = *arguments->socket;
            list_add(threadSubscribeList, structAppearedPokemon);
            log_info(optional_logger, "Queue subscription request APPEARED_POKEMON successfully.\n");
            connect_client(*arguments->socket,APPEARED_POKEMON);
            break;
        case CAUGHT_POKEMON:
            structCaughtPokemon = malloc(sizeof(threadSubscribe));
            structCaughtPokemon->idQueue = CAUGHT_POKEMON;
            structCaughtPokemon->socket = *arguments->socket;
            list_add(threadSubscribeList, structCaughtPokemon);
            log_info(optional_logger, "Queue subscription request CAUGHT_POKEMON successfully.\n");
            connect_client(*arguments->socket,CAUGHT_POKEMON);
            break;
        case LOCALIZED_POKEMON:
            structLocalizedPokemon = malloc(sizeof(threadSubscribe));
            structLocalizedPokemon->idQueue = LOCALIZED_POKEMON;
            structLocalizedPokemon->socket = *arguments->socket;
            list_add(threadSubscribeList, structLocalizedPokemon);
            log_info(optional_logger, "Queue subscription request LOCALIZED_POKEMON successfully.\n");
            connect_client(*arguments->socket,LOCALIZED_POKEMON);
            break;
        default:
            break;
    }
    
}

void connect_client(uint32_t socket,op_code codeOperation){
    
    t_process_request* process_request = malloc(sizeof(t_process_request)); 
    (*process_request).socket = malloc(sizeof(uint32_t));
    *(*process_request).socket = socket; 
    (*process_request).request_receiver = request;

    uint32_t id_connection = receive_connection_id(socket);

    switch(codeOperation){
        case APPEARED_POKEMON:
            structAppearedPokemon->idConnection = id_connection;
            break;
        case CAUGHT_POKEMON:
            structCaughtPokemon->idConnection = id_connection;
            break;
        case LOCALIZED_POKEMON:
            structLocalizedPokemon->idConnection = id_connection;
            break;
        default:
            break;
    }
    
    while(1){
        serve_client(process_request);
        close(socket);
        socket = crear_conexion(config_values.ip_broker, config_values.puerto_broker);
        process_request = malloc(sizeof(t_process_request)); 
        (*process_request).socket = malloc(sizeof(uint32_t));
        *(*process_request).socket = socket; 
        (*process_request).request_receiver = request;
        send_reconnect(socket, id_connection);
    } 

    pthread_join(server, NULL);
}

void listen_to_gameboy() { 
   start_server(config_values.ip_team, config_values.puerto_team, request);
   //pthread_create(&listening_gameboy, NULL, (void*)listen_to_gameboy, NULL);

}

void reception_message_queue_subscription(uint32_t code, uint32_t sizeofstruct, uint32_t client_fd) {
	void* stream = malloc(sizeofstruct);
    uint32_t* id_message = malloc(sizeof(uint32_t));
    uint32_t* id_message_correlational = malloc(sizeof(uint32_t));
    recv(client_fd, stream, sizeofstruct, MSG_WAITALL);

    switch(code){
        case APPEARED_POKEMON:;
            appeared_pokemon* appeared_pokemon_Message = stream_to_appeared_pokemon(stream, id_message, NULL, false); 
            appeared_pokemon_Message->pokemon = realloc(appeared_pokemon_Message->pokemon, appeared_pokemon_Message->sizePokemon+1);
            appeared_pokemon_Message->pokemon[appeared_pokemon_Message->sizePokemon] = '\0';
            log_info(obligatory_logger, "Receiving Message Appeared pokemon.");
            log_info(obligatory_logger, "Pokemon Appeared: %s, position: (%d,%d)", appeared_pokemon_Message->pokemon, appeared_pokemon_Message->position.posx, appeared_pokemon_Message->position.posy);
            send_ack(client_fd, *id_message);

            pokemonCompareGlobalObjetive = malloc(strlen(appeared_pokemon_Message->pokemon)+1);
            strcpy(pokemonCompareGlobalObjetive, appeared_pokemon_Message->pokemon);
            if(list_any_satisfy(globalObjetive, analyzePokemonInGlobal)){
                t_pokemon_on_map* newPokemonAppeared = malloc(sizeof(t_pokemon_on_map));
                newPokemonAppeared->state = P_FREE;
                newPokemonAppeared->position.posx = appeared_pokemon_Message->position.posx;
                newPokemonAppeared->position.posy = appeared_pokemon_Message->position.posy;
                newPokemonAppeared->pokemon = malloc(strlen(appeared_pokemon_Message->pokemon)+1);
                newPokemonAppeared->id = list_size(pokemonsOnMap) + 1;
                strcpy(newPokemonAppeared->pokemon, appeared_pokemon_Message->pokemon);

                list_add(pokemonsOnMap, newPokemonAppeared);

                sem_post(&plannerSemaphore);
            }
            free(pokemonCompareGlobalObjetive);
            break;
        case CAUGHT_POKEMON:;
			caught_pokemon* caught_Pokemon_Message = stream_to_caught_pokemon(stream, id_message, id_message_correlational, false);
            log_info(obligatory_logger, "Receiving Message Caught pokemon, Result %d", caught_Pokemon_Message->success);
            processCaughtPokemon(*id_message_correlational, caught_Pokemon_Message->success);
            
            send_ack(client_fd, *id_message);
            break;
        case LOCALIZED_POKEMON:;
            localized_pokemon* localized_Pokemon_Message = stream_to_localized_pokemon(stream, id_message, id_message_correlational, false);
            localized_Pokemon_Message->pokemon[localized_Pokemon_Message->sizePokemon] = '\0';
            int indexOfPokemonToLocalyze = getIndexPokemonToLocalizedByMessage(*id_message);
            if(indexOfPokemonToLocalyze != -1){
                list_remove(pokemonsToLocalize, indexOfPokemonToLocalyze);
                log_info(obligatory_logger, "Receiving Message Localized pokemon");
                log_info(obligatory_logger, "Pokemon %s", localized_Pokemon_Message->pokemon);
                
                for(int i = 0; i<list_size(localized_Pokemon_Message->positions); i++) {
                    t_position* positionAux = list_get(localized_Pokemon_Message->positions, i);
                    log_info(obligatory_logger,"Find in position: (%d,%d)", positionAux->posx, positionAux->posy);
                }
                send_ack(client_fd, *id_message);
            }
            
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

uint32_t caught_default() { 

    uint32_t success = 0;
    return success;
}

uint32_t localized_default(char* pokemon) {
    t_list* positions = list_create();
    localized_pokemon* loca=  init_localized_pokemon(pokemon, positions);
    loca->pokemon = pokemon;
    uint32_t position_default = list_size(loca->positions);
    return  position_default;
}

void* send_get_pokemon_global_team(){
    uint32_t client_fd = crear_conexion(config_values.ip_broker, config_values.puerto_broker);
    uint32_t* id_message = malloc(sizeof(uint32_t));
    get_pokemon* getPokemonMessage;
    void* stream;
    uint32_t total_global = list_size(globalObjetive);
    char* pokemonToSend;
    pokemonsToLocalize = list_create();
    
    for(int i=0; i< total_global; i++) {
        pokemonToSend = (char*)list_get(globalObjetive, i);
        getPokemonMessage = malloc(sizeof(get_pokemon));
        getPokemonMessage->pokemon = malloc(strlen(pokemonToSend));
        strcpy(getPokemonMessage->pokemon, pokemonToSend);
        getPokemonMessage->sizePokemon = strlen(getPokemonMessage->pokemon);
        *id_message = -1;
        stream =get_pokemon_to_stream(getPokemonMessage, id_message);

        t_paquete* packageToSend = stream_to_package(GET_POKEMON, stream, size_of_get_pokemon(getPokemonMessage));
        int bytes = packageToSend->buffer->size + 2*sizeof(uint32_t);
        void* buffer = (void *) serializar_paquete(packageToSend, bytes);

        send(client_fd, buffer, bytes, 0);
        free(buffer);
        log_info(optional_logger, "Pokemon %s: ", pokemonToSend);

        uint32_t sizeOfBuffer = sizeof(uint32_t) * 3;
        buffer = malloc(sizeOfBuffer);
        recv(client_fd, buffer, sizeOfBuffer, MSG_WAITALL);
        ack* acknowledgementMessage = stream_to_ack(buffer+8);
        log_info(optional_logger, "Id mensaje get: %d", acknowledgementMessage->id_message);

        addPokemonToLocalize(pokemonToSend, acknowledgementMessage->id_message);
    }
    return NULL;
}

void addPokemonToLocalize(char* pokemon, uint32_t idMessage){
    t_pokemonToLocalized* pokemonToLocalizeAux;
    bool pokemonExists = false;

    for(int i=0; i<list_size(pokemonsToLocalize) && !pokemonExists;i++){
        pokemonToLocalizeAux = (t_pokemonToLocalized*)list_get(pokemonsToLocalize, i);
        if(strcmp(pokemon, pokemonToLocalizeAux->pokemon) == 0){
            pokemonExists = true;
        }
    }

    if(!pokemonExists){
        pokemonToLocalizeAux = malloc(sizeof(t_pokemonToLocalized));
        pokemonToLocalizeAux->idMessage = idMessage;
        pokemonToLocalizeAux->pokemon = malloc(strlen(pokemon));
        strcpy(pokemonToLocalizeAux->pokemon, pokemon);
        list_add(pokemonsToLocalize, pokemonToLocalizeAux);
    }
}

int getIndexPokemonToLocalizedByMessage(uint32_t id_message){
    int result = -1;
    t_pokemonToLocalized* pokemonToLocalizeAux;

    for(int i=0; i<list_size(pokemonsToLocalize) && result == -1; i++){
        pokemonToLocalizeAux = (t_pokemonToLocalized*)list_get(pokemonsToLocalize, i);
        if(pokemonToLocalizeAux->idMessage == id_message){
            result = i;
        }
    }

    return result;
}

void processCaughtPokemon(uint32_t id_message, uint32_t success){
    t_threadTrainer* threadTrainerAux;
    for(int i=0; i<list_size(threadsTrainers); i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        if(threadTrainerAux->idMessageCatch == id_message){
            if(success == 1){
                //catch_succesfull
                catch_succesfull(threadTrainerAux->idTrainer);
                return;
            }else{
                //I switch state BLOCKED_BY_BROKER to BLOCKED
                //And delete pokemon
                threadTrainerAux->state = BLOCKED;
                removePokemonOnMap(threadTrainerAux->positionTo);
            }
        }
    }
}
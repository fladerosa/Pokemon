#include "server_gameboy.h"

void send_message(char** message, int socket_cliente,t_log*  optional_logger){
	t_paquete* paquete = malloc(sizeof(t_paquete));

    //A quien se lo mando
    char* receiver = message[1];

    //El nombre de la funcion esta en el parametro 2 del argv que le paso como parametro
    char* message_function = message[2]; 

    //Encuentro el op_code correspondiente con lo que recibi de mensaje.
    op_code operation_code;
    if(strcmp(message[1], "SUSCRIPTOR") == 0){
        operation_code = stringToEnum(message[1]);    
    }else{
        operation_code = stringToEnum(message_function);
    }
    
    paquete->buffer = malloc(sizeof(t_buffer));
    //Filtro por codigo de operacion segun el enum y creo el mensaje al servidor

    switch(operation_code){
        case NEW_POKEMON:;
             new_pokemon* newPokemonMessage = malloc(sizeof(new_pokemon)); 

             if(strcmp(receiver, "GAMECARD") == 0){
                 newPokemonMessage = init_new_pokemon(message[3], atoi(message[4]),atoi(message[5]), atoi(message[6]), atoi(message[7]));
             }else{
                 newPokemonMessage = init_new_pokemon(message[3], atoi(message[4]),atoi(message[5]), atoi(message[6]), -1);
             }
        
             paquete->codigo_operacion = NEW_POKEMON;
             paquete->buffer->size = sizeof(uint32_t) * 5 + strlen(newPokemonMessage->pokemon) + 1;
             paquete->buffer->stream = new_pokemon_to_stream(newPokemonMessage);
             break;
        case APPEARED_POKEMON:; 
            appeared_pokemon* appearedPokemonMessage = malloc(sizeof(appeared_pokemon));

            if(strcmp(receiver, "BROKER") == 0){
                appearedPokemonMessage = init_appeared_pokemon(message[3], atoi(message[4]), atoi(message[5]), -1, atoi(message[6]));
            }else{
                appearedPokemonMessage = init_appeared_pokemon(message[3], atoi(message[4]), atoi(message[5]), -1, -1);
            }

            paquete->codigo_operacion = APPEARED_POKEMON;
            paquete->buffer->size =  sizeof(uint32_t ) * 5 + strlen(appearedPokemonMessage->pokemon) + 1;
            paquete->buffer->stream = appeared_pokemon_to_stream(appearedPokemonMessage);
            break;
        case CATCH_POKEMON:;
            catch_pokemon* catchPokemonMessage = malloc(sizeof(catch_pokemon)); 

            if(strcmp(receiver, "GAMECARD") == 0){
                catchPokemonMessage = init_catch_pokemon(message[3], atoi(message[4]), atoi(message[5]), atoi(message[6]));
            }else{
                catchPokemonMessage = init_catch_pokemon(message[3], atoi(message[4]), atoi(message[5]), -1); 
            }

            paquete->codigo_operacion = CATCH_POKEMON;
            paquete->buffer->size = sizeof(uint32_t ) * 4 + strlen(catchPokemonMessage->pokemon) + 1;
            paquete->buffer->stream = catch_pokemon_to_stream(catchPokemonMessage);
            break;
        case CAUGHT_POKEMON:; 
            caught_pokemon* caughtPokemonMessage = malloc(sizeof(caught_pokemon)); 

            if(strcmp(message[4],"OK") == 0){
                caughtPokemonMessage = init_caught_pokemon(-1, atoi(message[3]), 1);
            }else{
                caughtPokemonMessage = init_caught_pokemon(-1, atoi(message[3]), 0);
            }

            paquete->codigo_operacion = CAUGHT_POKEMON;
            paquete->buffer->size = sizeof(uint32_t) * 3;
            paquete->buffer->stream = caught_pokemon_to_stream(caughtPokemonMessage);
            break;
        case GET_POKEMON:;
            get_pokemon* getPokemonMessage = malloc(sizeof(get_pokemon)); 

            getPokemonMessage = init_get_pokemon(message[3], -1);

            paquete->codigo_operacion = GET_POKEMON;
            paquete->buffer->size = sizeof(uint32_t ) * 2 + strlen(getPokemonMessage->pokemon) + 1;
            paquete->buffer->stream = get_pokemon_to_stream(getPokemonMessage);
            break;
        case SUSCRIPTOR:;
            subscribe* subscriber = malloc(sizeof(subscribe));

            subscriber = init_subscribe(stringToEnum(message[2]));

            paquete->codigo_operacion = SUSCRIPTOR; 
            paquete->buffer->size = sizeof(uint32_t);
            paquete->buffer->stream = subscribe_to_stream(subscriber);
            break;
        case ERROR:;
            paquete->codigo_operacion = ERROR;
            break;
        default:;
            log_info(optional_logger, "Incorrect format");
    }

	int bytes = paquete->buffer->size + 2*sizeof(uint32_t);

    //Creo el paquete y serializo 
	void* a_enviar = (void *) serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	free_package(paquete);
}

op_code stringToEnum(char* message_function){

    //Encuentro el string que corresponde al numero del enum
    for(int i = 0; i < 8; i++){
        if(strcmp(message_string[i].name, message_function) == 0){
            return message_string[i].operation;
        }
    }

    return ERROR;
}

void send_new_connection(uint32_t socket_broker){
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    new_connection* newConnection = init_new_connection();

    paquete->codigo_operacion = NEW_CONNECTION;
    paquete->buffer->size = sizeof(u_int32_t); // revisar
    paquete->buffer->stream = new_connection_to_stream(newConnection);

    uint32_t bytes = paquete->buffer->size + 2*sizeof(uint32_t);

    void* a_enviar = (void *) serializar_paquete(paquete, bytes);

	send(socket_broker, a_enviar, bytes, 0);

	free(a_enviar);
	free_package(paquete);
    free_new_connection(newConnection);
}

void send_reconnect(uint32_t socket_broker){
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    reconnect* reconnectToBroker = init_reconnect(socket_broker);

    paquete->codigo_operacion = RECONNECT;
    paquete->buffer->size = sizeof(u_int32_t); // revisar
    paquete->buffer->stream = reconnect_to_stream(reconnectToBroker);

    uint32_t bytes = paquete->buffer->size + 2*sizeof(uint32_t);

    void* a_enviar = (void *) serializar_paquete(paquete, bytes);

	send(socket_broker, a_enviar, bytes, 0);

	free(a_enviar);
	free_package(paquete);
    free_reconnect(reconnectToBroker);
}

void send_ack(uint32_t socket_broker, uint32_t id_message){
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    ack* ackBroker = malloc(sizeof(ack));

    ackBroker = init_ack(id_message);

    paquete->codigo_operacion = ACK; 
    paquete->buffer->size = sizeof(u_int32_t); // revisar
    paquete->buffer->stream = reconnect_to_stream(ackBroker);

    uint32_t bytes = paquete->buffer->size + 2*sizeof(uint32_t);

    void* a_enviar = (void *) serializar_paquete(paquete, bytes);

	send(socket_broker, a_enviar, bytes, 0);

	free(a_enviar);
    free_package(paquete);
    free_ack(ackBroker);
    
}

void receiveMessageSubscriptor(uint32_t cod_op, uint32_t sizeofstruct, uint32_t socketfd){

    void* stream = malloc(sizeofstruct);
    recv(socketfd, stream, sizeofstruct, MSG_WAITALL);
    
    switch(cod_op){
        case NEW_POKEMON:;
            new_pokemon* newPokemonMessage = stream_to_new_pokemon(stream);
            
            log_info(optional_logger, "New pokemon!");
            log_info(optional_logger, "This is the pokemon: %s", newPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the position x: %d", newPokemonMessage->position.posx);
            log_info(optional_logger, "This is the position y: %d", newPokemonMessage->position.posy);
            log_info(optional_logger, "This is the quantity: %d", newPokemonMessage->quantity);
            log_info(optional_logger, "This is the id message: %d", newPokemonMessage->id_message);
            send_ack(socketfd, newPokemonMessage->id_message);
            break;
        case APPEARED_POKEMON:;
            appeared_pokemon* appearedPokemonMessage = stream_to_appeared_pokemon(stream);

            log_info(optional_logger, "Appeared pokemon!");
            log_info(optional_logger, "This is the pokemon: %s", appearedPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the position x: %d", appearedPokemonMessage->position.posx);
            log_info(optional_logger, "This is the position y: %d", appearedPokemonMessage->position.posy);
            log_info(optional_logger, "This is the id correlational: %d", appearedPokemonMessage->id_correlational);
            log_info(optional_logger, "This is the id message: %d", appearedPokemonMessage->id_message);
            send_ack(socketfd, appearedPokemonMessage->id_message);
            break;
        case CATCH_POKEMON:;

            catch_pokemon* catchPokemonMessage = stream_to_catch_pokemon(stream);

            log_info(optional_logger, "Catch pokemon!");
            log_info(optional_logger, "This is the pokemon: %s", catchPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the position x: %d", catchPokemonMessage->position.posx);
            log_info(optional_logger, "This is the position y: %d", catchPokemonMessage->position.posy);
            log_info(optional_logger, "This is the id message: %d", catchPokemonMessage->id_message);
            send_ack(socketfd, catchPokemonMessage->id_message);
            break;
        case CAUGHT_POKEMON:;

            caught_pokemon* caughtPokemonMessage = stream_to_caught_pokemon(stream);

            char* wasCaught; 

            if(caughtPokemonMessage->success){
                wasCaught = "OK";
            }else{
                wasCaught = "FAIL";
            }

            log_info(optional_logger, "Caught pokemon!");
            log_info(optional_logger, "Was the pokemon catch?: %d", wasCaught); 
            log_info(optional_logger, "This is the id correlational: %d", caughtPokemonMessage->id_correlational);
            log_info(optional_logger, "This is the id message: %d", caughtPokemonMessage->id_message);
            send_ack(socketfd, caughtPokemonMessage->id_message);
            break;
        case GET_POKEMON:;

            get_pokemon* getPokemonMessage = stream_to_get_pokemon(stream); 

            log_info(optional_logger, "Get pokemon!");
            log_info(optional_logger, "This is the pokemon: %s", getPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the id message: %d", getPokemonMessage->id_message);
            send_ack(socketfd, getPokemonMessage->id_message);
            break;
        case LOCALIZED_POKEMON:;

            localized_pokemon* localizedPokemonMessage = stream_to_localized_pokemon(stream);

            log_info(optional_logger, "Localized pokemon!");
            log_info(optional_logger, "This is the pokemon: %s", localizedPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the size of the list of positions: %d", (*localizedPokemonMessage->positions).elements_count);
            log_info(optional_logger, "This is the id correlational: %d", localizedPokemonMessage->id_correlational);
            log_info(optional_logger, "This is the id message: %d", localizedPokemonMessage->id_message);
            send_ack(socketfd, localizedPokemonMessage->id_message);
            break;
        case SUSCRIPTOR:; 

            subscribe* subscribeMessage = stream_to_subscribe(stream);

            log_info(optional_logger, "Subscribe!");
            log_info(optional_logger, "This is the queue: %d", subscribeMessage->colaMensajes);            
            break;
        case NEW_CONNECTION:; 

            new_connection* newConnectionMessage = stream_to_new_connection(stream);
            free(newConnectionMessage);
            log_info(optional_logger, "New connection!");
            break;
        case CONNECTION:;

            connection* connectionMessage = stream_to_connection(stream);

            log_info(optional_logger, "Connection!"); 
            log_info(optional_logger, "This is the id connection: %d", connectionMessage->id_connection);
            break;
        case RECONNECT:;

            reconnect* reconnectMessage = stream_to_reconnect(stream);

            log_info(optional_logger, "Reconnect!");
            log_info(optional_logger, "This is the id connection: %d",reconnectMessage->id_connection);
            break; 
        case ACK:;

            ack* acknowledgementMessage = stream_to_ack(stream);

            log_info(optional_logger, "Acknowledgement!");
            log_info(optional_logger, "This is the id message: %d", acknowledgementMessage->id_message);
            break;
        case -1:;
            break;
        default:;
            log_info(optional_logger, "Cannot understand op_code received.");
    }
}

uint32_t receive_connection_id(uint32_t socket_broker){
    uint32_t codop, size, id_connection;
    recv(socket_broker, &codop, sizeof(uint32_t), MSG_WAITALL);
    recv(socket_broker, &size, sizeof(uint32_t), MSG_WAITALL);
    recv(socket_broker, &id_connection, sizeof(uint32_t), MSG_WAITALL);

    return id_connection;
}
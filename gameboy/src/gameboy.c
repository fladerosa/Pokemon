#include "gameboy.h"

void send_message(char** message, int socket_cliente,t_log*  optional_logger)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

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
        case 1:
             paquete->codigo_operacion = NEW_POKEMON;
             paquete->buffer->size = sizeof(new_pokemon);
             new_pokemon newPokemonMessage; 
             newPokemonMessage.pokemon = message[3];
             newPokemonMessage.sizePokemon = strlen(newPokemonMessage.pokemon) + 1;
             newPokemonMessage.posx = (uint32_t) message[4];
             newPokemonMessage.posy = (uint32_t) message[5];
             newPokemonMessage.quantity = (uint32_t) message[6];
             paquete->buffer->stream = malloc(paquete->buffer->size);
	         memcpy(paquete->buffer->stream, &newPokemonMessage, paquete->buffer->size);
             break;
        case 2: 
            paquete->codigo_operacion = APPEARED_POKEMON;
            paquete->buffer->size = sizeof(appeared_pokemon);
            appeared_pokemon appearedPokemonMessage; 
            appearedPokemonMessage.pokemon = message[3];
            appearedPokemonMessage.sizePokemon = strlen(appearedPokemonMessage.pokemon) + 1;
            appearedPokemonMessage.posx = (uint32_t) message[4];
            appearedPokemonMessage.posy = (uint32_t) message[5];
            appearedPokemonMessage.id_message = (uint32_t) message[6];
            paquete->buffer->stream = malloc(paquete->buffer->size);
	        memcpy(paquete->buffer->stream, &appearedPokemonMessage, paquete->buffer->size);
            break;
        case 3: 
            paquete->codigo_operacion = CATCH_POKEMON;
            paquete->buffer->size = sizeof(catch_pokemon);
            catch_pokemon catchPokemonMessage; 
            catchPokemonMessage.pokemon = message[3];
            catchPokemonMessage.sizePokemon = strlen(catchPokemonMessage.pokemon) + 1;
            catchPokemonMessage.posx = (uint32_t) message[4];
            catchPokemonMessage.posy = (uint32_t) message[5];
            paquete->buffer->stream = malloc(paquete->buffer->size);
	        memcpy(paquete->buffer->stream, &catchPokemonMessage, paquete->buffer->size);
            break;
        case 4: 
            paquete->codigo_operacion = CAUGHT_POKEMON;
            paquete->buffer->size = sizeof(caught_pokemon);
            caught_pokemon caughtPokemonMessage; 
            caughtPokemonMessage.id_message = (uint32_t) message[3];
            caughtPokemonMessage.caught = message[4];
            caughtPokemonMessage.sizeCaught = strlen(caughtPokemonMessage.caught) + 1;
            paquete->buffer->stream = malloc(paquete->buffer->size);
	        memcpy(paquete->buffer->stream, &caughtPokemonMessage, paquete->buffer->size);
            break;
        case 5: 
            paquete->codigo_operacion = GET_POKEMON;
            paquete->buffer->size = sizeof(get_pokemon);
            get_pokemon getPokemonMessage; 
            getPokemonMessage.pokemon = message[3];
            getPokemonMessage.sizePokemon = strlen(getPokemonMessage.pokemon) + 1;
            paquete->buffer->stream = malloc(paquete->buffer->size);
	        memcpy(paquete->buffer->stream, &getPokemonMessage, paquete->buffer->size);
            break;
        case 6:
            paquete->codigo_operacion = SUSCRIPTOR; 
            paquete->buffer->size = sizeof(subscriptor);
            subscriptor suscriptor;
            suscriptor.colaMensajes = message[2];
            paquete->buffer->stream = malloc(paquete->buffer->size);
            memcpy(paquete->buffer->stream, &suscriptor, paquete->buffer->size);
            break; 
        case 7:
            paquete->codigo_operacion = ERROR;
            break;
        case 8: //Para el op_code MENSAJE, despues borrar!!!!
            break;
    }

	int bytes = paquete->buffer->size + 2*sizeof(int);

    //Creo el paquete y serializo 
	void* a_enviar = (void *) serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

op_code stringToEnum(char* message_function){

    //Encuentro el string que corresponde al numero del enum
    for(int i = 0; i < sizeof(op_code); i++){
        if(strcmp(message_string[i].name, message_function) == 0){
            return message_string[i].operation;
        }
    }

    return ERROR;
}

void closeAll(t_log* optional_logger,t_log* obligatory_logger, t_config* config, int connection){
    
    //Destruyo todo: logger opcional, logger obligatorio, config y conexion.
    log_destroy(optional_logger);
    log_destroy(obligatory_logger);
    config_destroy(config);
    close(connection);
}

void receiveMessageSubscriptor(uint32_t cod_op, uint32_t sizeofstruct, uint32_t socketfd){

    new_pokemon newPokemonMessage;
    appeared_pokemon appearedPokemonMessage;
    catch_pokemon catchPokemonMessage;
    caught_pokemon caughtPokemonMessage;
    get_pokemon getPokemonMessage;

    switch(cod_op){
        case 1: 
            recv(socketfd, (void*) newPokemonMessage.sizePokemon, sizeof(uint32_t), MSG_WAITALL);
            newPokemonMessage.pokemon = malloc(newPokemonMessage.sizePokemon);
            recv(socketfd, (void*) newPokemonMessage.pokemon, newPokemonMessage.sizePokemon, MSG_WAITALL);
            recv(socketfd, (void*) newPokemonMessage.posx, sizeof(uint32_t), MSG_WAITALL);
            recv(socketfd, (void*) newPokemonMessage.posy, sizeof(uint32_t), MSG_WAITALL);
            recv(socketfd, (void*) newPokemonMessage.quantity, sizeof(uint32_t), MSG_WAITALL);
            
            log_info(optional_logger, "New pokemon!");
            log_info(optional_logger, "This is the pokemon: ", newPokemonMessage.pokemon); 
            log_info(optional_logger, "This is the position x: ", newPokemonMessage.posx);
            log_info(optional_logger, "This is the position y: ", newPokemonMessage.posy);
            log_info(optional_logger, "This is the quantity: ", newPokemonMessage.quantity);
            break;
        case 2:
            recv(socketfd, (void*) appearedPokemonMessage.sizePokemon, sizeof(uint32_t), MSG_WAITALL);
            appearedPokemonMessage.pokemon = malloc(appearedPokemonMessage.sizePokemon);
            recv(socketfd, (void*) appearedPokemonMessage.pokemon, appearedPokemonMessage.sizePokemon, MSG_WAITALL);
            recv(socketfd, (void*) appearedPokemonMessage.posx, sizeof(uint32_t), MSG_WAITALL);
            recv(socketfd, (void*) appearedPokemonMessage.posy, sizeof(uint32_t), MSG_WAITALL);
            recv(socketfd, (void*) appearedPokemonMessage.id_message, sizeof(uint32_t), MSG_WAITALL);
            
            log_info(optional_logger, "Appeared pokemon!");
            log_info(optional_logger, "This is the pokemon: ", appearedPokemonMessage.pokemon); 
            log_info(optional_logger, "This is the position x: ", appearedPokemonMessage.posx);
            log_info(optional_logger, "This is the position y: ", appearedPokemonMessage.posy);
            log_info(optional_logger, "This is the id message: ", appearedPokemonMessage.id_message);
            break;
        case 3:
            recv(socketfd, (void*) catchPokemonMessage.sizePokemon, sizeof(uint32_t), MSG_WAITALL);
            catchPokemonMessage.pokemon = malloc(catchPokemonMessage.sizePokemon);
            recv(socketfd, (void*) catchPokemonMessage.pokemon, catchPokemonMessage.sizePokemon, MSG_WAITALL);
            recv(socketfd, (void*) catchPokemonMessage.posx, sizeof(uint32_t), MSG_WAITALL);
            recv(socketfd, (void*) catchPokemonMessage.posy, sizeof(uint32_t), MSG_WAITALL);
            
            log_info(optional_logger, "Catch pokemon!");
            log_info(optional_logger, "This is the pokemon: ", appearedPokemonMessage.pokemon); 
            log_info(optional_logger, "This is the position x: ", appearedPokemonMessage.posx);
            log_info(optional_logger, "This is the position y: ", appearedPokemonMessage.posy);
            break;
        case 4:
            recv(socketfd, (void*) caughtPokemonMessage.sizeCaught, sizeof(uint32_t), MSG_WAITALL);
            caughtPokemonMessage.caught = malloc(sizeof(caughtPokemonMessage.sizeCaught));
            recv(socketfd, (void*) caughtPokemonMessage.caught, sizeof(caughtPokemonMessage.sizeCaught), MSG_WAITALL);
            recv(socketfd, (void*) caughtPokemonMessage.id_message, sizeof(uint32_t), MSG_WAITALL);
            
            log_info(optional_logger, "Caught pokemon!");
            log_info(optional_logger, "Was the pokemon catch?: ", caughtPokemonMessage.caught); 
            log_info(optional_logger, "This is the id message: ", appearedPokemonMessage.id_message);
            break;
        case 5:
            recv(socketfd, (void*) getPokemonMessage.sizePokemon, sizeof(uint32_t), MSG_WAITALL);
            getPokemonMessage.pokemon = malloc(getPokemonMessage.sizePokemon);
            recv(socketfd, (void*) getPokemonMessage.pokemon, getPokemonMessage.sizePokemon, MSG_WAITALL);
            
            log_info(optional_logger, "Get pokemon!");
            log_info(optional_logger, "This is the pokemon: ", getPokemonMessage.pokemon); 
            break;
    }
    

}

int main(int argc, char ** argv){

    //Inicializando la config
    config = config_create("./cfg/gameboy.config");

    int showConsole = config_get_int_value(config,"LOG_SHOW");

    //Inicializando el logger
    obligatory_logger = log_create("./cfg/obligatory.log", "obligatory", showConsole, LOG_LEVEL_INFO); 
    optional_logger = log_create("./cfg/optional.log", "optional", showConsole, LOG_LEVEL_INFO);

    char* ip; 
    char* port;

    char* server = argv[1];
    //Busco el ip y el puerto
    if(strcmp(server,"BROKER") == 0 || strcmp(server, "SUSCRIPTOR") == 0){
        ip = config_get_string_value(config, "IP_BROKER");
        port = config_get_string_value(config, "PUERTO_BROKER");
    }else if(strcmp(server,"TEAM") == 0){
        ip = config_get_string_value(config, "IP_TEAM");
        port = config_get_string_value(config, "PUERTO_TEAM");
    }else if(strcmp(server,"GAMECARD") == 0){
        ip = config_get_string_value(config, "IP_GAMECARD");
        port = config_get_string_value(config, "PUERTO_GAMECARD");
    }else{
        log_error(optional_logger,"No se pudo conectar");
        exit(4);
    }

    //Creo la conexion
    int connection = crear_conexion(ip, port);

    //Mando el mensaje
    send_message(argv, connection, optional_logger);

    if(strcmp(server, "SUSCRIPTOR")){
        uint32_t time = (uint32_t) argv[3];
        t_process_request* process_request = malloc(sizeof(t_process_request)); 
        (*process_request).socket = connection; 
        (*process_request).request_receiver = receiveMessageSubscriptor;

       while(time > 0){
            serve_client(process_request);
            time--; 
       } 
    }

    //Cierro y elimino todo
    closeAll(optional_logger, obligatory_logger,config,connection);
    return 0;
}
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
        case NEW_POKEMON:;
             new_pokemon* newPokemonMessage = malloc(sizeof(new_pokemon)); 
             newPokemonMessage->pokemon = message[3];
             newPokemonMessage->sizePokemon = strlen(newPokemonMessage->pokemon) + 1;
             newPokemonMessage->position.posx = atoi(message[4]);
             newPokemonMessage->position.posy = atoi(message[5]);
             newPokemonMessage->quantity = atoi(message[6]);
             newPokemonMessage->id_message = -1; 
             paquete->codigo_operacion = NEW_POKEMON;
             paquete->buffer->size = sizeof(uint32_t) * 5 + strlen(newPokemonMessage->pokemon) + 1;
             paquete->buffer->stream = new_pokemon_to_stream(newPokemonMessage);
             break;
        case APPEARED_POKEMON:; 
            appeared_pokemon* appearedPokemonMessage = malloc(sizeof(appeared_pokemon));
            appearedPokemonMessage->pokemon = message[3];
            appearedPokemonMessage->sizePokemon = strlen(appearedPokemonMessage->pokemon) + 1;
            appearedPokemonMessage->position.posx = atoi(message[4]);
            appearedPokemonMessage->position.posy = atoi(message[5]);
            appearedPokemonMessage->id_correlational = atoi(message[6]);
            appearedPokemonMessage->id_message = -1;
            paquete->codigo_operacion = APPEARED_POKEMON;
            paquete->buffer->size =  sizeof(uint32_t ) * 5 + strlen(appearedPokemonMessage->pokemon) + 1;
            paquete->buffer->stream = appeared_pokemon_to_stream(appearedPokemonMessage);
            break;
        case CATCH_POKEMON:;
            catch_pokemon* catchPokemonMessage = malloc(sizeof(catch_pokemon)); 
            catchPokemonMessage->pokemon = message[3];
            catchPokemonMessage->sizePokemon = strlen(catchPokemonMessage->pokemon) + 1;
            catchPokemonMessage->position.posx = atoi(message[4]);
            catchPokemonMessage->position.posy = atoi(message[5]);
            catchPokemonMessage->id_message = -1; 
            paquete->codigo_operacion = CATCH_POKEMON;
            paquete->buffer->size = sizeof(uint32_t ) * 4 + strlen(catchPokemonMessage->pokemon) + 1;
            paquete->buffer->stream = catch_pokemon_to_stream(catchPokemonMessage);
            break;
        case CAUGHT_POKEMON:; 
            caught_pokemon* caughtPokemonMessage = malloc(sizeof(caught_pokemon)); 
            caughtPokemonMessage->id_correlational = atoi(message[3]);
            if(strcmp(message[4],"OK") == 0){
                caughtPokemonMessage->success = 1;
            }else{
                caughtPokemonMessage->success = 0;
            }
            caughtPokemonMessage->id_message = -1;
            paquete->codigo_operacion = CAUGHT_POKEMON;
            paquete->buffer->size = sizeof(uint32_t) * 3;
            paquete->buffer->stream = caught_pokemon_to_stream(caughtPokemonMessage);
            break;
        case GET_POKEMON:;
            get_pokemon* getPokemonMessage = malloc(sizeof(get_pokemon)); 
            getPokemonMessage->pokemon = message[3];
            getPokemonMessage->sizePokemon = strlen(getPokemonMessage->pokemon) + 1;
            getPokemonMessage->id_message = -1; 
            paquete->codigo_operacion = GET_POKEMON;
            paquete->buffer->size = sizeof(uint32_t ) * 2 + strlen(getPokemonMessage->pokemon) + 1;
            paquete->buffer->stream = get_pokemon_to_stream(getPokemonMessage);
            break;
        case SUSCRIPTOR:;
            subscribe *subscriber = malloc(sizeof(subscribe));
            subscriber->colaMensajes = stringToEnum(message[2]);
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
    for(int i = 0; i < 8; i++){
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

void countTime(void* timePassed){ 
    sleep((uint32_t) timePassed);
    exit(EXIT_SUCCESS);
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

    if(strcmp(server, "SUSCRIPTOR") == 0){
       
        t_process_request* process_request = malloc(sizeof(t_process_request)); 
        (*process_request).socket = connection; 
        (*process_request).request_receiver = receiveMessageSubscriptor;
    
        pthread_t threadConnection;
        pthread_create(&threadConnection, NULL, (void*) countTime, (void*) atoi(argv[3]));

        while(1){
            serve_client(process_request);
        }     
    }

    //Cierro y elimino todo
    closeAll(optional_logger, obligatory_logger,config,connection);
    return 0;
}
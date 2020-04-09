#include "gameboy.h"

void send_message(char** message, int socket_cliente,t_log*  optional_logger)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

    //El nombre de la funcion esta en el parametro 2 del argv que le paso como parametro
    char* message_function = message[2]; 
    op_code operation_code = stringToEnum(message_function);
    paquete->buffer = malloc(sizeof(t_buffer));
    switch(operation_code){
        case 1:
             paquete->codigo_operacion = NEW_POKEMON;
             paquete->buffer->size = sizeof(new_pokemon);
             new_pokemon newPokemonMessage; 
             newPokemonMessage.pokemon = message[3];
             newPokemonMessage.posx = message[4];
             newPokemonMessage.posy = message[5];
             newPokemonMessage.quantity = message[6];
             paquete->buffer->stream = malloc(paquete->buffer->size);
	         memcpy(paquete->buffer->stream, &newPokemonMessage, paquete->buffer->size);
             break;
        case 2: 
            paquete->codigo_operacion = APPEARED_POKEMON;
            paquete->buffer->size = sizeof(appeared_pokemon);
            appeared_pokemon appearedPokemonMessage; 
            appearedPokemonMessage.pokemon = message[3];
            appearedPokemonMessage.posx = message[4];
            appearedPokemonMessage.posy = message[5];
            appearedPokemonMessage.id_message = message[6];
            paquete->buffer->stream = malloc(paquete->buffer->size);
	        memcpy(paquete->buffer->stream, &appearedPokemonMessage, paquete->buffer->size);
            break;
        case 3: 
            paquete->codigo_operacion = CATCH_POKEMON;
            paquete->buffer->size = sizeof(catch_pokemon);
            catch_pokemon catchPokemonMessage; 
            catchPokemonMessage.pokemon = message[3];
            catchPokemonMessage.posx = message[4];
            catchPokemonMessage.posy = message[5];
            paquete->buffer->stream = malloc(paquete->buffer->size);
	        memcpy(paquete->buffer->stream, &catchPokemonMessage, paquete->buffer->size);
            break;
        case 4: 
            paquete->codigo_operacion = CAUGHT_POKEMON;
            paquete->buffer->size = sizeof(caught_pokemon);
            caught_pokemon caughtPokemonMessage; 
            caughtPokemonMessage.id_message = message[3];
            caughtPokemonMessage.caught = message[4];
            paquete->buffer->stream = malloc(paquete->buffer->size);
	        memcpy(paquete->buffer->stream, &caughtPokemonMessage, paquete->buffer->size);
            break;
        case 5: 
            paquete->codigo_operacion = GET_POKEMON;
            paquete->buffer->size = sizeof(get_pokemon);
            get_pokemon getPokemonMessage; 
            getPokemonMessage.pokemon = message[3];
            paquete->buffer->stream = malloc(paquete->buffer->size);
	        memcpy(paquete->buffer->stream, &getPokemonMessage, paquete->buffer->size);
            break;
        case 6:
            paquete->codigo_operacion = ERROR;
            break;
    }

	int bytes = paquete->buffer->size + 2*sizeof(int);

	/*void* a_enviar = (void *) serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);*/
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

op_code stringToEnum(char* message_function){

    for(int i = 0; i < sizeof(op_code); i++){
        if(strcmp(message_string[i].name, message_function) == 0){
            return message_string[i].operation;
        }
    }

    return ERROR;
}

void closeAll(t_log* optional_logger,t_log* obligatory_logger, t_config* config, int connection){
    log_destroy(optional_logger);
    log_destroy(obligatory_logger);
    config_destroy(config);
    close(connection);
}

int main(int argc, char ** argv){

    t_log* obligatory_logger;
    t_log* optional_logger; 

    t_config* config;

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
    if(strcmp(server,"BROKER") == 0){
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
    int connection = create_connection(ip, port);

    //Mando el mensaje
    send_message(argv,connection, optional_logger);

    //Cierro y elimino todo
    closeAll(optional_logger, obligatory_logger,config,connection);
    return 0;
}
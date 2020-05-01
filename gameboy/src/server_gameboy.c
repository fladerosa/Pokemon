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
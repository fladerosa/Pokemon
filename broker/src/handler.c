#include "handler.h"

void process_request(uint32_t cod_op, uint32_t sizeofstruct, uint32_t client_fd) {
	void* stream = malloc(sizeofstruct);
    recv(client_fd, stream, sizeofstruct, MSG_WAITALL);
    
    switch(cod_op){
        case NEW_POKEMON:;
            new_pokemon* newPokemonMessage = stream_to_new_pokemon(stream);
            
            log_info(optional_logger, "New pokemon!");
            log_info(optional_logger, "This is the pokemon: %s", newPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the position x: %d", newPokemonMessage->position.posx);
            log_info(optional_logger, "This is the position y: %d", newPokemonMessage->position.posy);
            log_info(optional_logger, "This is the quantity: %d", newPokemonMessage->quantity);
            log_info(optional_logger, "This is the id message: %d", newPokemonMessage->id_message);
            break;
        case APPEARED_POKEMON:;
            appeared_pokemon* appearedPokemonMessage = stream_to_appeared_pokemon(stream);

            log_info(optional_logger, "Appeared pokemon!");
            log_info(optional_logger, "This is the pokemon: %s", appearedPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the position x: %d", appearedPokemonMessage->position.posx);
            log_info(optional_logger, "This is the position y: %d", appearedPokemonMessage->position.posy);
            log_info(optional_logger, "This is the id correlational: %d", appearedPokemonMessage->id_correlational);
            log_info(optional_logger, "This is the id message: %d", appearedPokemonMessage->id_message);
            break;
        case CATCH_POKEMON:;

            catch_pokemon* catchPokemonMessage = stream_to_catch_pokemon(stream);

            log_info(optional_logger, "Catch pokemon!");
            log_info(optional_logger, "This is the pokemon: %s", catchPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the position x: %d", catchPokemonMessage->position.posx);
            log_info(optional_logger, "This is the position y: %d", catchPokemonMessage->position.posy);
            log_info(optional_logger, "This is the id message: %d", catchPokemonMessage->id_message);
            break;
        case CAUGHT_POKEMON:;

            caught_pokemon* caughtPokemonMessage = stream_to_caught_pokemon(stream);

            log_info(optional_logger, "Caught pokemon!");
            log_info(optional_logger, "Was the pokemon catch?: %d", caughtPokemonMessage->success); 
            log_info(optional_logger, "This is the id correlational: %d", caughtPokemonMessage->id_correlational);
            log_info(optional_logger, "This is the id message: %d", caughtPokemonMessage->id_message);
            break;
        case GET_POKEMON:;

            get_pokemon* getPokemonMessage = stream_to_get_pokemon(stream); 

            log_info(optional_logger, "Get pokemon!");
            log_info(optional_logger, "This is the pokemon: %s", getPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the id message: %d", getPokemonMessage->id_message);
            break;
        case LOCALIZED_POKEMON:;

            localized_pokemon* localizedPokemonMessage = stream_to_localized_pokemon(stream);

            log_info(optional_logger, "Localized pokemon!");
            log_info(optional_logger, "This is the pokemon: %s", localizedPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the size of the list of positions: %d", (*localizedPokemonMessage->positions).elements_count);
            log_info(optional_logger, "This is the id correlational: %d", localizedPokemonMessage->id_correlational);
            log_info(optional_logger, "This is the id message: %d", localizedPokemonMessage->id_message);
            break;
        case SUSCRIPTOR:; 

            subscribe* subscribeMessage = stream_to_subscribe(stream);

            log_info(optional_logger, "Subscribe!");
            log_info(optional_logger, "This is the queue: %d", subscribeMessage->colaMensajes);            
            break;
        case NEW_CONNECTION:; 
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
		case MENSAJE:;
			process_message(client_fd, stream);
        case -1:
            break;
    }
}


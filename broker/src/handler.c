#include "handler.h"

void process_request(uint32_t cod_op, uint32_t sizeofstruct, uint32_t client_fd) {
	void* stream = malloc(sizeofstruct);
    recv(client_fd, stream, sizeofstruct, MSG_WAITALL);
    
    switch(cod_op){
        case NEW_POKEMON:;
            new_pokemon* newPokemonMessage = stream_to_new_pokemon(stream);
            break;
        case APPEARED_POKEMON:;
            appeared_pokemon* appearedPokemonMessage = stream_to_appeared_pokemon(stream);            free(newPokemonMessage);
            break;
        case CATCH_POKEMON:;
            catch_pokemon* catchPokemonMessage = stream_to_catch_pokemon(stream);

            break;
        case CAUGHT_POKEMON:;
            caught_pokemon* caughtPokemonMessage = stream_to_caught_pokemon(stream);

            break;
        case GET_POKEMON:;
            get_pokemon* getPokemonMessage = stream_to_get_pokemon(stream); 

            break;
        case LOCALIZED_POKEMON:;
            localized_pokemon* localizedPokemonMessage = stream_to_localized_pokemon(stream);

            break;
        case SUSCRIPTOR:; 
            subscribe* subscribeMessage = stream_to_subscribe(stream);

            break;
        case NEW_CONNECTION:; 
            break;
        case CONNECTION:;
            connection* connectionMessage = stream_to_connection(stream);

            break;
        case RECONNECT:;
            reconnect* reconnectMessage = stream_to_reconnect(stream);

            break; 
        case ACK:;
            ack* acknowledgementMessage = stream_to_ack(stream);
            
            break;
		case MENSAJE:;
			process_message(client_fd, stream);
        case -1:
            break;
    }
}


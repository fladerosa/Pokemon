#include "handler.h"

void process_request(uint32_t cod_op, uint32_t sizeofstruct, uint32_t client_fd) {
	void* stream = malloc(sizeofstruct);
    if (recv(client_fd, stream, sizeofstruct, MSG_WAITALL)<=0){free(stream); return;}
    
    switch(cod_op){
        case NEW_POKEMON:;
            new_pokemon* newPokemonMessage = stream_to_new_pokemon(stream);
            newPokemonMessage->id_message = get_id_message();
            add_message_to_queue(
                newPokemonMessage, 
                NEW_POKEMON,
                size_of_new_pokemon(newPokemonMessage),
                newPokemonMessage->id_message);
            break;
        case APPEARED_POKEMON:;
            appeared_pokemon* appearedPokemonMessage = stream_to_appeared_pokemon(stream);
            appearedPokemonMessage->id_message = get_id_message();
            add_message_to_queue(
                appearedPokemonMessage, 
                APPEARED_POKEMON,
                size_of_appeared_pokemon(appearedPokemonMessage),
                appearedPokemonMessage->id_message);
            break;
        case CATCH_POKEMON:;
            catch_pokemon* catchPokemonMessage = stream_to_catch_pokemon(stream);
            catchPokemonMessage->id_message = get_id_message();
            add_message_to_queue(
                catchPokemonMessage, 
                CATCH_POKEMON,
                size_of_catch_pokemon(catchPokemonMessage),
                catchPokemonMessage->id_message);
            break;
        case CAUGHT_POKEMON:;
            caught_pokemon* caughtPokemonMessage = stream_to_caught_pokemon(stream);
            caughtPokemonMessage->id_message = get_id_message();
            add_message_to_queue(
                caughtPokemonMessage, 
                CAUGHT_POKEMON,
                size_of_caught_pokemon(caughtPokemonMessage),
                caughtPokemonMessage->id_message);
            break;
        case GET_POKEMON:;
            get_pokemon* getPokemonMessage = stream_to_get_pokemon(stream); 
            getPokemonMessage->id_message = get_id_message();
            add_message_to_queue(
                getPokemonMessage, 
                GET_POKEMON,
                size_of_get_pokemon(getPokemonMessage),
                getPokemonMessage->id_message);
            break;
        case LOCALIZED_POKEMON:;
            localized_pokemon* localizedPokemonMessage = stream_to_localized_pokemon(stream);
            localizedPokemonMessage->id_message = get_id_message();
            add_message_to_queue(
                localizedPokemonMessage, 
                LOCALIZED_POKEMON,
                size_of_localized_pokemon(localizedPokemonMessage),
                localizedPokemonMessage->id_message);
            break;
        case SUSCRIPTOR:; 
            subscribe* subscribeMessage = stream_to_subscribe(stream);
            handle_subscribe(client_fd, subscribeMessage);
            break;
        case NEW_CONNECTION:;
            handle_new_connection(client_fd); 
            break;
        case RECONNECT:;
            reconnect* reconnectMessage = stream_to_reconnect(stream);
            handle_reconnect(client_fd,reconnectMessage);
            break; 
        case ACK:;
            ack* acknowledgementMessage = stream_to_ack(stream);
            handle_ack(client_fd, acknowledgementMessage);
            break;
		case MENSAJE:;
			process_message(client_fd, stream);
        case -1:
            break;
        default:;
            log_info(optional_logger,"Received invalid operation code.");
    }

    free(stream);
}


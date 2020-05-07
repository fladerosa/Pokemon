#include "handler.h"

void process_request(uint32_t cod_op, uint32_t sizeofstruct, uint32_t client_fd) {
	void* stream = malloc(sizeofstruct);
    if (recv(client_fd, stream, sizeofstruct, MSG_WAITALL)<=0){free(stream); return;}
    switch(cod_op){
        case NEW_POKEMON:;
            handle_new_pokemon(stream, client_fd);
            break;
        case APPEARED_POKEMON:;
            handle_appeared_pokemon(stream, client_fd);
            break;
        case CATCH_POKEMON:;
            handle_catch_pokemon(stream, client_fd);
            break;
        case CAUGHT_POKEMON:;
            handle_caught_pokemon(stream, client_fd);
            break;
        case GET_POKEMON:;
            handle_get_pokemon(stream, client_fd);
            break;
        case LOCALIZED_POKEMON:;
            handle_localized_pokemon(stream, client_fd);
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


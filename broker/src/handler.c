#include "handler.h"

void process_request(uint32_t cod_op, uint32_t sizeofstream, uint32_t client_fd) {
	void* stream = malloc(sizeofstream);
    if (recv(client_fd, stream, sizeofstream, MSG_WAITALL)<=0){free(stream); return;}
    switch(cod_op){
        case NEW_POKEMON:;
        case APPEARED_POKEMON:;
        case CATCH_POKEMON:;
        case CAUGHT_POKEMON:;
        case GET_POKEMON:;
        case LOCALIZED_POKEMON:;
            t_data* message =  assign_and_return_message(cod_op, sizeofstream, stream);
            if (message != NULL){
                send_ack(client_fd, message->id);
                add_message_to_queue(message, cod_op);
            }
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


#include "queues.h"

uint32_t get_id_message(){ //crear mutex al llamar a esto
    static uint32_t id_message = 1;
    return id_message++;
}

uint32_t get_id_connection(){ // y esto
    static uint32_t id_connection = 1;
    return id_connection++;
}

void* list_find_with_args(t_list *self, bool(*condition)(void*, void*), void* args) {
	t_link_element *element = self->head;
	int position = 0;

	while (element != NULL && !condition(element->data, args)) {
		element = element->next;
		position++;
	}

	return element != NULL ? element->data : NULL;
}

void handle_new_connection(uint32_t client_fd){
    t_connection *conn = malloc(sizeof(t_connection));
    conn->socket = client_fd;
    pthread_mutex_lock(&m_id_connection);
    conn->id_connection = get_id_connection();
    pthread_mutex_unlock(&m_id_connection);
    conn->is_connected = true;
    list_add(connections, conn);

    connection* response_conn = init_connection(conn->id_connection);
    void* stream = connection_to_stream(response_conn);
    uint32_t size = sizeof(uint32_t)*3;
    t_paquete* package = stream_to_package(CONNECTION, stream, sizeof(uint32_t));
    void* response = serializar_paquete(package, size );
    send(client_fd, response, size, 0);
    free_connection(response_conn);
    free_package(package);
}

bool has_connection_id(void* data, void* id){
    return ((t_connection*) data)->id_connection == (uint32_t) id;
}

bool has_socket_fd(void* data, void* socket){
    return data ? ((t_connection*) data)->socket == (uint32_t) socket : false;
}

bool has_queue_id(void* data, void* id){
    return data ? ((t_message_queue*) data)->id_queue == (uint32_t) id : false;
}

void handle_reconnect(uint32_t client_fd, reconnect* reconn){
    t_connection* conn = (t_connection*) 
        list_find_with_args(
            connections, 
            has_connection_id, 
            (void*) reconn->id_connection
        );
    if (conn){
        conn->socket = client_fd;
        conn->is_connected = true;
    } else {
        handle_new_connection(client_fd);
    }
}

void handle_subscribe(uint32_t client_fd, subscribe* subs){
    t_message_queue* queue = list_find_with_args(list_queues, has_queue_id, (void*) subs->colaMensajes);
    t_connection* conn = list_find_with_args(connections, has_socket_fd,(void*)client_fd);
    if (queue && conn){
        list_add(queue->subscribers, conn);
    }
}

void* connection_to_receiver(void* connection){
    t_receiver* receiver = malloc(sizeof(t_receiver));
    receiver->conn = (t_connection*) connection;
    receiver->sent = false;
    receiver->received = false;
    return (void*) receiver;
}

void add_message_to_queue(void* data, op_code code, uint32_t size_of_data, uint32_t id_message, uint32_t id_correlational){
    t_message_queue* queue = list_find_with_args(list_queues, has_queue_id,(void*) code);
    t_message* new_message = malloc(sizeof(t_message));
    new_message->data = data;
    new_message->size = size_of_data;
    new_message->receivers = list_map(queue->subscribers, connection_to_receiver);
    new_message->id_message = id_message;
    new_message->id_correlational = id_correlational;
    new_message->id_queue = code;
    pthread_mutex_lock(queue->m_queue_modify);
    queue_push(queue->messages, new_message);
    pthread_mutex_unlock(queue->m_queue_modify);
    sem_post(queue->sem_message);
}

void* get_stream_by_queue_id(t_message* message,uint32_t queue_id){
    void* stream = NULL;
    switch(queue_id){
        case NEW_POKEMON:;
            stream = new_pokemon_to_stream(
                message->data, 
                &(message->id_message)
            );
            break;
        case APPEARED_POKEMON:;
            stream = appeared_pokemon_to_stream(
                message->data, 
                &(message->id_message), 
                &(message->id_correlational)
            );
            break;
        case CATCH_POKEMON:;
            stream = catch_pokemon_to_stream(
                message->data,
                &(message->id_message)
            );
            break;
        case CAUGHT_POKEMON:;
            stream = caught_pokemon_to_stream(
                message->data,
                &(message->id_message),
                &(message->id_correlational)
            );
            break;
        case GET_POKEMON:;
            stream = get_pokemon_to_stream(
                message->data,
                &(message->id_message)
            );
            break;
        case LOCALIZED_POKEMON:;
            stream = localized_pokemon_to_stream(
                message->data,
                &(message->id_message),
                &(message->id_correlational)
            );
            break;
        default:;
            log_info(optional_logger, "Error: Queue has wrong id");
    }
    return stream;
}

void queue_message_sender(void* args){
    t_message_queue* queue = (t_message_queue*) args;
    //log_info(optional_logger, "Starting queue number %d", queue->id_queue);
    while(1){   
        sem_wait(queue->sem_message);
        pthread_mutex_lock(queue->m_queue_modify);
        t_message* message = queue_peek(queue->messages);
        pthread_mutex_unlock(queue->m_queue_modify);
        for(int i=0; i<list_size(message->receivers); i++){
            t_receiver* receiver = list_get(message->receivers, i);
            if(receiver->conn->is_connected){
                void* stream = get_stream_by_queue_id(message, queue->id_queue);
                t_paquete* package = stream_to_package(queue->id_queue, stream, message->size);
                void* a_enviar = serializar_paquete(package,sizeof(uint32_t)*2 + package->buffer->size);
                send(receiver->conn->socket, a_enviar, sizeof(uint32_t)*2 + package->buffer->size, 0);
                receiver->sent = true;
            }
        }
        sem_wait(queue->sem_all_ack);
        pthread_mutex_lock(queue->m_queue_modify);
        queue_pop(queue->messages);
        pthread_mutex_unlock(queue->m_queue_modify);
    }
}

bool has_message_id(void* message, void* id){
    return message ? ((t_message*) message)->id_message == (uint32_t) id : false;
}

void* queue_to_first_message(void* queue){
    return queue_peek(((t_message_queue*) queue)->messages);
}

bool hasReceived(void* receiver){
    t_receiver* rec = (t_receiver*) receiver;
    return rec ? rec->conn->is_connected && rec->received : false; 
}

bool receiver_has_socket_fd(void* receiver, void* socket){
    return receiver ? 
        ((t_receiver*) receiver)->conn->socket == (uint32_t) socket 
        : false;
}

void handle_ack(uint32_t client_fd, ack* acknowledgement){
    t_list* first_messages = list_map(list_queues, queue_to_first_message);
    t_message* message = list_find_with_args(first_messages, has_message_id, (void*) acknowledgement->id_message);
    if(message){
        t_receiver* receiver = list_find_with_args(
            message->receivers, 
            receiver_has_socket_fd,
            (void*) client_fd);
        if(receiver){
            receiver->received = true;
            if (list_all_satisfy(message->receivers, hasReceived)){
                t_message_queue* queue = list_find_with_args(list_queues, has_queue_id,(void*) message->id_queue);
                sem_post(queue->sem_all_ack);
            }
        }
    }
}

void handle_new_pokemon(void* stream, uint32_t client_fd){
    uint32_t id_message = -1, id_correlational = -1;
    new_pokemon* newPokemonMessage 
        = stream_to_new_pokemon(
            stream, NULL, true);
    pthread_mutex_lock(&m_id_message);
    id_message = get_id_message();
    pthread_mutex_unlock(&m_id_message);
    send_ack(client_fd, id_message);
    add_message_to_queue(
        newPokemonMessage, 
        NEW_POKEMON,
        size_of_new_pokemon(newPokemonMessage),
        id_message,
        id_correlational);
}

void handle_appeared_pokemon(void* stream, uint32_t client_fd){
    uint32_t id_message = -1, id_correlational = -1;
    appeared_pokemon* appearedPokemonMessage 
        = stream_to_appeared_pokemon(
            stream, NULL, &id_correlational, true);
    pthread_mutex_lock(&m_id_message);
    id_message = get_id_message();
    pthread_mutex_unlock(&m_id_message);
    send_ack(client_fd, id_message);
    add_message_to_queue(
        appearedPokemonMessage, 
        APPEARED_POKEMON,
        size_of_appeared_pokemon(appearedPokemonMessage),
        id_message,
        id_correlational);
}

void handle_catch_pokemon(void* stream, uint32_t client_fd){
    uint32_t id_message = -1, id_correlational = -1;
    catch_pokemon* catchPokemonMessage = 
        stream_to_catch_pokemon(stream, NULL, true);
    pthread_mutex_lock(&m_id_message);
    id_message = get_id_message();
    pthread_mutex_unlock(&m_id_message);
    send_ack(client_fd, id_message);
    add_message_to_queue(
        catchPokemonMessage, 
        CATCH_POKEMON,
        size_of_catch_pokemon(catchPokemonMessage),
        id_message,
        id_correlational);
}

void handle_caught_pokemon(void* stream, uint32_t client_fd){
    uint32_t id_message = -1, id_correlational = -1;
    caught_pokemon* caughtPokemonMessage 
        = stream_to_caught_pokemon(
            stream, NULL, &id_correlational, true);
    pthread_mutex_lock(&m_id_message);
    id_message = get_id_message();
    pthread_mutex_unlock(&m_id_message);
    send_ack(client_fd, id_message);
    add_message_to_queue(
        caughtPokemonMessage, 
        CAUGHT_POKEMON,
        size_of_caught_pokemon(caughtPokemonMessage),
        id_message,
        id_correlational);
}

void handle_get_pokemon(void* stream, uint32_t client_fd){
    uint32_t id_message = -1, id_correlational = -1;
    get_pokemon* getPokemonMessage 
        = stream_to_get_pokemon(stream, NULL, true); 
    pthread_mutex_lock(&m_id_message);
    id_message = get_id_message();
    pthread_mutex_unlock(&m_id_message);
    send_ack(client_fd, id_message);
    add_message_to_queue(
        getPokemonMessage, 
        GET_POKEMON,
        size_of_get_pokemon(getPokemonMessage),
        id_message,
        id_correlational);
}

void handle_localized_pokemon(void* stream, uint32_t client_fd){
    uint32_t id_message = -1, id_correlational = -1;
    localized_pokemon* localizedPokemonMessage 
        = stream_to_localized_pokemon(
            stream, NULL, &id_correlational, true);
    pthread_mutex_lock(&m_id_message);
    id_message = get_id_message();
    pthread_mutex_unlock(&m_id_message);
    send_ack(client_fd, id_message);
    add_message_to_queue(
        localizedPokemonMessage, 
        LOCALIZED_POKEMON,
        size_of_localized_pokemon(localizedPokemonMessage),
        id_message,
        id_correlational);
}

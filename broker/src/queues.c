#include "queues.h"

uint32_t get_id_message(){
    pthread_mutex_lock(&m_id_message);
    static uint32_t id_message = 0;
    id_message++;
    pthread_mutex_unlock(&m_id_message);
    return id_message;
}

uint32_t get_id_connection(){
    pthread_mutex_lock(&m_id_connection);
    static uint32_t id_connection = 0;
    id_connection++;
    pthread_mutex_unlock(&m_id_connection);
    return id_connection;
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
    conn->id_connection = get_id_connection();
    conn->is_connected = true;
    list_add(connections, conn);

    connection* response_conn = init_connection(conn->id_connection);
    void* stream = connection_to_stream(response_conn);
    uint32_t size = sizeof(uint32_t)*2 + sizeof(stream);
    t_paquete* package = stream_to_package(CONNECTION, connection_to_stream(response_conn));
    void* response = serializar_paquete(package, size );
    send(client_fd, response, size, 0);
}

bool has_connection_id(void* data, void* id){
    return ((t_connection*) data)->id_connection == (uint32_t) id;
}

bool has_queue_id(void* data, void* id){
    return ((t_message_queue*) data)->id_queue == (uint32_t) id;
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
    t_connection* conn = list_find_with_args(connections, has_connection_id,(void*)client_fd);
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

void add_message_to_queue(void* data, op_code code){
    t_message_queue* queue = list_find_with_args(list_queues, has_queue_id,(void*) code);
    t_message* new_message = malloc(sizeof(t_message));
    new_message->data = data;
    new_message->receivers = list_map(queue->subscribers, connection_to_receiver);
    //puede que necesitemos mutex aca
    queue_push(queue->messages, new_message);
}


void queue_message_sender(void* args){
    t_message_queue* queue = (t_message_queue*) args;
    while(1){
        //semaforos
        t_message* message = queue_pop(queue->messages);
        for(int i=0; i<list_size(message->receivers); i++){
            
        }
    }
}

void handle_ack(uint32_t client_fd, ack* acknowledgement){
    
}
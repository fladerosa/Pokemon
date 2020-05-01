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

t_link_element* list_find_element_with_args(t_list *self, bool(*condition)(void*, void*), void* args) {
	t_link_element *element = self->head;
	int position = 0;

	while (element != NULL && !condition(element->data, args)) {
		element = element->next;
		position++;
	}

	return element;
}

void* list_find_with_args(t_list *self, bool(*condition)(void*, void*), void* args) {
	t_link_element *element = list_find_element_with_args(self, condition, args);
	return element != NULL ? element->data : NULL;
}


void handle_new_connection(uint32_t client_fd){
    t_connection *conn = malloc(sizeof(t_connection));
    conn->socket = client_fd;
    conn->id_connection = get_id_connection();
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

void handle_reconnect(uint32_t client_fd, reconnect* reconn){
    t_connection* conn = (t_connection*) 
        list_find_with_args(
            connections, 
            has_connection_id, 
            (void*) reconn->id_connection
        );
    if (conn){
        conn->socket = client_fd;
    }
}
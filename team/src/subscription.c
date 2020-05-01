#include "subscription.h"

void listen_to_gameboy()
{ 
   start_server(values.ip_team, values.puerto_team, request);
   //mandar mensaje de confirmacion cuando el gameboy me mande appeared_pokemon()
}

void establish_broker_connection()
{
    char *mensaje= "SUSCRIPCION GLOBAL";

    uint32_t server_connection = crear_conexion(values.ip_broker, values.puerto_broker);
  //validar resultados

    send_subscription_queue_message_appeared_pokemon(mensaje, server_connection, obligatory_logger);
    send_subscription_queue_message_catch_pokemon(mensaje, server_connection, obligatory_logger);
    send_subscription_queue_message_caught_pokemon(mensaje, server_connection, obligatory_logger);
    send_subscription_queue_message_get_pokemon(mensaje, server_connection, obligatory_logger);
    send_subscription_queue_message_localized_pokemon(mensaje, server_connection, obligatory_logger);

    pthread_t connection_broker;

    
}

void send_subscription_queue_message_appeared_pokemon(char* mensaje, uint32_t server_connection, t_log* obligatory_logger)
{
    t_buffer* buffer = (t_buffer*) malloc(sizeof(buffer));
	buffer->size = strlen(mensaje)+1;
	void* stream = malloc(buffer->size);
	uint32_t offset = 0;
	strcpy(stream + offset,mensaje);
	buffer->stream = stream;
	t_paquete* paquete = (t_paquete*) malloc(sizeof(t_paquete)); 
    paquete->codigo_operacion = APPEARED_POKEMON;  
    paquete->buffer = buffer; 	
    //serializar
	void* to_send = stream_to_appeared_pokemon(stream);
	send(server_connection, to_send, buffer->size + sizeof(uint32_t)*2, MSG_WAITALL);
    log_info(obligatory_logger, "Requesting subscription to Appeared_Pokemon queue\n");
}

void send_subscription_queue_message_catch_pokemon(char* mensaje, uint32_t server_connection, t_log* obligatory_logger)
{
    t_buffer* buffer = (t_buffer*) malloc(sizeof(buffer));
	buffer->size = strlen(mensaje)+1;
	void* stream = malloc(buffer->size);
	uint32_t offset = 0;
	strcpy(stream + offset,mensaje);
	buffer->stream = stream;
	t_paquete* paquete = (t_paquete*) malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CATCH_POKEMON;
	paquete->buffer = buffer;
    //serializar
	void* to_send = stream_to_catch_pokemon(stream);
	send(server_connection, to_send, buffer->size + sizeof(uint32_t)*2, MSG_WAITALL);
    log_info(obligatory_logger, "Requesting subscription to Catch_Pokemon queue\n");
}

void send_subscription_queue_message_caught_pokemon(char* mensaje, uint32_t server_connection, t_log* obligatory_logger)
{
    t_buffer* buffer = (t_buffer*) malloc(sizeof(buffer));
	buffer->size = strlen(mensaje)+1;
	void* stream = malloc(buffer->size);
	uint32_t offset = 0;
	strcpy(stream + offset,mensaje);
	buffer->stream = stream;
	t_paquete* paquete = (t_paquete*) malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CAUGHT_POKEMON;
	paquete->buffer = buffer;
    //serializar
	void* to_send = stream_to_caught_pokemon(stream);
	send(server_connection, to_send, buffer->size + sizeof(uint32_t)*2, MSG_WAITALL);
    log_info(obligatory_logger, "Requesting subscription to Caught_Pokemon queue\n");
}

void send_subscription_queue_message_get_pokemon(char* mensaje, uint32_t server_connection, t_log* obligatory_logger)
{
    t_buffer* buffer = (t_buffer*) malloc(sizeof(buffer));
	buffer->size = strlen(mensaje)+1;
	void* stream = malloc(buffer->size);
	uint32_t offset = 0;
	strcpy(stream + offset,mensaje);
	buffer->stream = stream;
	t_paquete* paquete = (t_paquete*) malloc(sizeof(t_paquete));
	paquete->codigo_operacion = GET_POKEMON;
	paquete->buffer = buffer;
    //serializar
	void* to_send = stream_to_get_pokemon(stream);
	send(server_connection, to_send, buffer->size + sizeof(uint32_t)*2, MSG_WAITALL);
    log_info(obligatory_logger, "Requesting subscription to Get_Pokemon queue\n");
}
void send_subscription_queue_message_localized_pokemon(char* mensaje, uint32_t server_connection, t_log* obligatory_logger)
{
    t_buffer* buffer = (t_buffer*) malloc(sizeof(buffer));
	buffer->size = strlen(mensaje)+1;
	void* stream = malloc(buffer->size);
	uint32_t offset = 0;
	strcpy(stream + offset,mensaje);
	buffer->stream = stream;
	t_paquete* paquete = (t_paquete*) malloc(sizeof(t_paquete));
	paquete->codigo_operacion = LOCALIZED_POKEMON;
	paquete->buffer = buffer;
    //serializar
	void* to_send = stream_to_localized_pokemon(stream);
	send(server_connection, to_send, buffer->size + sizeof(uint32_t)*2, MSG_WAITALL);
    log_info(obligatory_logger, "Requesting subscription to Localized_Pokemon queue\n");
}
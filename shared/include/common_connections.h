#ifndef COMMON_CONNECTIONS_H
#define COMMON_CONNECTIONS_H

#include "common_utils.h"

typedef void (*on_request)(int codop, int client);

typedef struct
{
	int* socket;
	on_request request_receiver;
} t_process_request;

typedef enum
{
	MENSAJE=1
} op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

void  receive_new_connections(int socket_escucha, on_request request_receiver);
void  start_server(char* ip, char* port, on_request request_receiver);
void  serve_client(t_process_request* processor);
void  devolver_mensaje(void* payload, int size, int socket_cliente);
void* serializar_paquete(t_paquete* paquete, int bytes);
void* recibir_mensaje(int socket_cliente, int* size);
void  process_request(int cod_op, int cliente_fd);


#endif
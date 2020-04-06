#include "common_connections.h"

void start_server(char* ip, char* port, on_request request_receiver){
    int socket_servidor;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, port, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            log_error(optional_logger, "Could not create socket.");
            continue;
        }

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            log_error(optional_logger, "Could not bind socket.");
            continue;
        }
        break;
    }
	listen(socket_servidor, SOMAXCONN);
    log_info(optional_logger, "Started listening on %s:%s", ip, port);

    freeaddrinfo(servinfo);

    while(1)
    	receive_new_connections(socket_servidor, request_receiver);
}

void receive_new_connections(int socket_escucha, on_request request_receiver){
    int connfd;
    struct sockaddr_in cli; 
    unsigned int len = sizeof(cli); 
  
    connfd = accept(socket_escucha, (struct sockaddr*)&cli, &len);
    if (connfd < 0) { 
        log_info(optional_logger, "Server accept failed..."); 
    } 
    else {
        log_info(optional_logger, "Server accepted a new client...");
        t_process_request processor;
        processor.socket = &socket_escucha;
        processor.request_receiver = request_receiver;
        pthread_create(&client_listener, NULL, 
            (void*) serve_client, 
            &processor);
    }
}

void serve_client(t_process_request* processor)
{
    int* socket = (*processor).socket;
    on_request request_receiver = (*processor).request_receiver;
	int cod_op;
	if(recv(*(*processor).socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	log_info(optional_logger, "Received op_code: %d by socket: %d", cod_op, *socket);
	request_receiver(cod_op, *socket);
}

void* recibir_mensaje(int socket_cliente, int* size)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	log_info(optional_logger, "The stream size is %d bytes", *size);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);
	return buffer;
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void devolver_mensaje(void* payload, int size, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = size;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, payload, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
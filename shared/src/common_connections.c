#include "common_connections.h"

void start_server(char* ip, char* port, on_request request_receiver){
    uint32_t socket_servidor;
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
    log_info(optional_logger, "Starting new listening thread on %s:%s", ip, port);

    freeaddrinfo(servinfo);
	t_process_request* server_processor = malloc(sizeof(t_process_request));
	(*server_processor).socket = socket_servidor;
	(*server_processor).request_receiver = request_receiver;
	pthread_create(&server, NULL, (void*)run_server, server_processor);
}

void run_server(void * server_processor){
    mask_sig();
	uint32_t socket = (*(t_process_request*)server_processor).socket;
	listen(socket, SOMAXCONN);
	on_request request_receiver = (*(t_process_request*)server_processor).request_receiver;
	while(true){
		receive_new_connections(socket,request_receiver);
	}
}

void receive_new_connections(uint32_t socket_escucha, on_request request_receiver){
    uint32_t connfd;
    struct sockaddr_in cli; 
    uint32_t len = sizeof(cli); 
  
    connfd = accept(socket_escucha, (struct sockaddr*)&cli, &len);
    if (connfd < 0) { 
        log_info(optional_logger, "Server accept failed..."); 
    } else {
        log_info(optional_logger, "Server accepted a new client on socket: %d", connfd);
        t_process_request processor;
        processor.socket = connfd;
        processor.request_receiver = request_receiver;
        pthread_create_and_detach( 
            (void*) serve_client, 
            &processor);
    }
}

void serve_client(t_process_request* processor){
    mask_sig();
    uint32_t socket = processor->socket, size = -1, cod_op=-1;
    on_request request_receiver = (*processor).request_receiver;
	while(1){
        if(recv(socket,(void*) &cod_op, sizeof(uint32_t), MSG_WAITALL)<=0) break;
        log_info(optional_logger, "Received op_code: %d by socket: %d", cod_op, socket);
        if(recv(socket,(void*) &size, sizeof(uint32_t), MSG_WAITALL)<=0) break;
        log_info(optional_logger, "Size of stream: %d", size);
        request_receiver(cod_op, size, socket);
    }
    log_info(optional_logger, "Socket %d has disconnected", socket);
    close(socket);
}

t_paquete* stream_to_package(op_code code,void* payload){
    t_paquete* package = malloc(sizeof(t_paquete));
    package->buffer = malloc(sizeof(t_buffer));
    package->codigo_operacion = code;
    package->buffer->size = sizeof(payload);
    package->buffer->stream = payload;
    return package;
}

void* serializar_paquete(t_paquete* paquete, uint32_t bytes){
	void * magic = malloc(bytes);
	uint32_t desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(uint32_t));
	desplazamiento+= sizeof(uint32_t);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void devolver_mensaje(void* payload, uint32_t socket_cliente){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = sizeof(payload);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, payload, paquete->buffer->size);

	uint32_t bytes = paquete->buffer->size + 2*sizeof(uint32_t);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

uint32_t crear_conexion(char *ip, char* puerto){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	uint32_t socketfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

    while(connect(socketfd, server_info->ai_addr, server_info->ai_addrlen) == -1){
        log_info(optional_logger, "Could not connect to server on %s:%s.", ip, puerto);
        uint32_t timeConfig = config_get_int_value(config,"CONNECTION_TIME");
        log_info(optional_logger, "Trying to connect again in: %d", timeConfig);
        sleep(timeConfig);
    }

	log_info(optional_logger, "Connected successfully with %s:%s.", ip, puerto);
		
	freeaddrinfo(server_info);

	return socketfd;
}


void enviar_mensaje(char* mensaje, uint32_t socketfd){
	t_buffer* buffer = (t_buffer*) malloc(sizeof(buffer));
	buffer->size = strlen(mensaje)+1;
	void* stream = malloc(buffer->size);
	uint32_t offset = 0;
	strcpy(stream + offset,mensaje);
	buffer->stream = stream;
	t_paquete* paquete = (t_paquete*) malloc(sizeof(t_paquete));
	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = buffer;
	void* a_enviar = serializar_paquete(paquete, buffer->size + sizeof(uint32_t)*2);
	send(socketfd, a_enviar, buffer->size + sizeof(uint32_t)*2, MSG_WAITALL);
}


void process_message(uint32_t client_fd, void* stream){
	void* msg;
	msg = (char*) stream;
	log_info(optional_logger, "The message received is: %s", (char*)msg);
	devolver_mensaje(msg, client_fd);
	free(msg);
}

void pthread_create_and_detach(void* function, void* args ){
	pthread_t thread;
	pthread_create(&thread, NULL, function, args);
	pthread_detach(thread);
}

void free_package(t_paquete* package){
    free(package->buffer->stream);
    free(package->buffer);
    free(package);
}

void mask_sig(void)
{
	sigset_t mask;
	sigemptyset(&mask); 
    sigaddset(&mask, SIGUSR1);         
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
}

void* suscribirseA(op_code codigoOp,uint32_t socket_broker){
    subscribe* suscripcion = init_subscribe(codigoOp);
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->codigo_operacion = SUSCRIPTOR;
    paquete->buffer->size = sizeof(uint32_t);
    paquete->buffer->stream = subscribe_to_stream(suscripcion);
    uint32_t bytes = paquete->buffer->size + 2*sizeof(uint32_t);
    void* a_enviar = (void *) serializar_paquete(paquete, bytes);
	send(socket_broker, a_enviar, bytes, 0);
    free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
    return NULL;
}

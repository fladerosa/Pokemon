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
	listen(socket_servidor, SOMAXCONN);
    log_info(optional_logger, "Started listening on %s:%s", ip, port);

    freeaddrinfo(servinfo);
	t_process_request* server_processor = malloc(sizeof(t_process_request));
	(*server_processor).socket = socket_servidor;
	(*server_processor).request_receiver = request_receiver;
	pthread_create(&server, NULL, (void*)run_server, server_processor);
}

void run_server(void * server_processor){
	uint32_t socket = (*(t_process_request*)server_processor).socket;
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
        log_info(optional_logger, "Server accepted a new client...");
        t_process_request processor;
        processor.socket = connfd;
        processor.request_receiver = request_receiver;
        pthread_create_and_detach( 
            (void*) serve_client, 
            &processor);
    }
}

void serve_client(t_process_request* processor){
    uint32_t socket = (*processor).socket;
    uint32_t size = -1;
    on_request request_receiver = (*processor).request_receiver;
	uint32_t cod_op=-1;
	recv(socket,(void*) &cod_op, sizeof(uint32_t), MSG_WAITALL);
	log_info(optional_logger, "Received op_code: %d by socket: %d", cod_op, socket);
    recv(socket,(void*) &size, sizeof(uint32_t), MSG_WAITALL);
	log_info(optional_logger, "Size of stream: %d", size);
    request_receiver(cod_op, size, socket);
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

void devolver_mensaje(void* payload, uint32_t size, uint32_t socket_cliente){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = size;
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


char* recibir_mensaje(uint32_t socketfd, uint32_t buffer_size){
	void* stream = malloc(buffer_size);
	recv(socketfd, stream, buffer_size, 0);
	return (char*) stream;
}

void process_message(uint32_t client_fd, uint32_t size){
	void* msg;
	msg = recibir_mensaje(client_fd, size);
	log_info(optional_logger, "The message received is: %s", (char*)msg);
	devolver_mensaje(msg, size, client_fd);
	free(msg);
}

void pthread_create_and_detach(void* function, void* args ){
	pthread_t thread;
	pthread_create(&thread, NULL, function, args);
	pthread_detach(thread);
}

void receiveMessageSubscriptor(uint32_t cod_op, uint32_t sizeofstruct, uint32_t socketfd){

    new_pokemon newPokemonMessage;
    appeared_pokemon appearedPokemonMessage;
    catch_pokemon catchPokemonMessage;
    caught_pokemon caughtPokemonMessage;
    get_pokemon getPokemonMessage; 

    newPokemonMessage.sizePokemon = -1; 
    appearedPokemonMessage.sizePokemon = -1;
    catchPokemonMessage.sizePokemon = -1;
    caughtPokemonMessage.sizeCaught = -1;

    switch(cod_op){
        case 1: 
            recv(socketfd, (void*) newPokemonMessage.sizePokemon, sizeof(uint32_t), MSG_WAITALL);
            newPokemonMessage.pokemon = malloc(newPokemonMessage.sizePokemon);
            recv(socketfd, (void*) newPokemonMessage.pokemon, newPokemonMessage.sizePokemon, MSG_WAITALL);
            recv(socketfd, (void*) newPokemonMessage.posx, sizeof(uint32_t), MSG_WAITALL);
            recv(socketfd, (void*) newPokemonMessage.posy, sizeof(uint32_t), MSG_WAITALL);
            recv(socketfd, (void*) newPokemonMessage.quantity, sizeof(uint32_t), MSG_WAITALL);
            
            log_info(optional_logger, "New pokemon!");
            log_info(optional_logger, "This is the pokemon: ", newPokemonMessage.pokemon); 
            log_info(optional_logger, "This is the position x: ", newPokemonMessage.posx);
            log_info(optional_logger, "This is the position y: ", newPokemonMessage.posy);
            log_info(optional_logger, "This is the quantity: ", newPokemonMessage.quantity);
            break;
        case 2:
            recv(socketfd, (void*) appearedPokemonMessage.sizePokemon, sizeof(uint32_t), MSG_WAITALL);
            appearedPokemonMessage.pokemon = malloc(appearedPokemonMessage.sizePokemon);
            recv(socketfd, (void*) appearedPokemonMessage.pokemon, appearedPokemonMessage.sizePokemon, MSG_WAITALL);
            recv(socketfd, (void*) appearedPokemonMessage.posx, sizeof(uint32_t), MSG_WAITALL);
            recv(socketfd, (void*) appearedPokemonMessage.posy, sizeof(uint32_t), MSG_WAITALL);
            recv(socketfd, (void*) appearedPokemonMessage.id_message, sizeof(uint32_t), MSG_WAITALL);
            
            log_info(optional_logger, "Appeared pokemon!");
            log_info(optional_logger, "This is the pokemon: ", appearedPokemonMessage.pokemon); 
            log_info(optional_logger, "This is the position x: ", appearedPokemonMessage.posx);
            log_info(optional_logger, "This is the position y: ", appearedPokemonMessage.posy);
            log_info(optional_logger, "This is the id message: ", appearedPokemonMessage.id_message);
            break;
        case 3:
            recv(socketfd, (void*) catchPokemonMessage.sizePokemon, sizeof(uint32_t), MSG_WAITALL);
            catchPokemonMessage.pokemon = malloc(catchPokemonMessage.sizePokemon);
            recv(socketfd, (void*) catchPokemonMessage.pokemon, catchPokemonMessage.sizePokemon, MSG_WAITALL);
            recv(socketfd, (void*) catchPokemonMessage.posx, sizeof(uint32_t), MSG_WAITALL);
            recv(socketfd, (void*) catchPokemonMessage.posy, sizeof(uint32_t), MSG_WAITALL);
            
            log_info(optional_logger, "Catch pokemon!");
            log_info(optional_logger, "This is the pokemon: ", appearedPokemonMessage.pokemon); 
            log_info(optional_logger, "This is the position x: ", appearedPokemonMessage.posx);
            log_info(optional_logger, "This is the position y: ", appearedPokemonMessage.posy);
            break;
        case 4:
            recv(socketfd, (void*) caughtPokemonMessage.sizeCaught, sizeof(uint32_t), MSG_WAITALL);
            caughtPokemonMessage.caught = malloc(sizeof(caughtPokemonMessage.sizeCaught));
            recv(socketfd, (void*) caughtPokemonMessage.caught, sizeof(caughtPokemonMessage.sizeCaught), MSG_WAITALL);
            recv(socketfd, (void*) caughtPokemonMessage.id_message, sizeof(uint32_t), MSG_WAITALL);
            
            log_info(optional_logger, "Caught pokemon!");
            log_info(optional_logger, "Was the pokemon catch?: ", caughtPokemonMessage.caught); 
            log_info(optional_logger, "This is the id message: ", appearedPokemonMessage.id_message);
            break;
        case 5:
            recv(socketfd, (void*) getPokemonMessage.sizePokemon, sizeof(uint32_t), MSG_WAITALL);
            getPokemonMessage.pokemon = malloc(getPokemonMessage.sizePokemon);
            recv(socketfd, (void*) getPokemonMessage.pokemon, getPokemonMessage.sizePokemon, MSG_WAITALL);
            
            log_info(optional_logger, "Get pokemon!");
            log_info(optional_logger, "This is the pokemon: ", getPokemonMessage.pokemon); 
            break;
        case -1:
            break;
    }
}
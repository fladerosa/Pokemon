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

void receiveMessageSubscriptor(uint32_t cod_op, uint32_t sizeofstruct, uint32_t socketfd){

    void* stream = malloc(sizeofstruct);
    recv(socketfd, stream, sizeofstruct, MSG_WAITALL);
    
    switch(cod_op){
        case NEW_POKEMON:;
            new_pokemon* newPokemonMessage = stream_to_new_pokemon(stream);
            
            log_info(optional_logger, "New pokemon!");
            log_info(optional_logger, "This is the pokemon: ", newPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the position x: ", newPokemonMessage->position.posx);
            log_info(optional_logger, "This is the position y: ", newPokemonMessage->position.posy);
            log_info(optional_logger, "This is the quantity: ", newPokemonMessage->quantity);
            log_info(optional_logger, "This is the id message: ", newPokemonMessage->id_message);
            break;
        case APPEARED_POKEMON:;
            appeared_pokemon* appearedPokemonMessage = stream_to_appeared_pokemon(stream);

            log_info(optional_logger, "Appeared pokemon!");
            log_info(optional_logger, "This is the pokemon: ", appearedPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the position x: ", appearedPokemonMessage->position.posx);
            log_info(optional_logger, "This is the position y: ", appearedPokemonMessage->position.posy);
            log_info(optional_logger, "This is the id correlational: ", appearedPokemonMessage->id_correlational);
            log_info(optional_logger, "This is the id message: ", appearedPokemonMessage->id_message);
            break;
        case CATCH_POKEMON:;

            catch_pokemon* catchPokemonMessage = stream_to_catch_pokemon(stream);

            log_info(optional_logger, "Catch pokemon!");
            log_info(optional_logger, "This is the pokemon: ", catchPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the position x: ", catchPokemonMessage->position.posx);
            log_info(optional_logger, "This is the position y: ", catchPokemonMessage->position.posy);
            log_info(optional_logger, "This is the id message: ", catchPokemonMessage->id_message);
            break;
        case CAUGHT_POKEMON:;

            caught_pokemon* caughtPokemonMessage = stream_to_caught_pokemon(stream);

            char* wasCaught; 

            if(caughtPokemonMessage->success){
                wasCaught = "OK";
            }else{
                wasCaught = "FAIL";
            }

            log_info(optional_logger, "Caught pokemon!");
            log_info(optional_logger, "Was the pokemon catch?: ", wasCaught); 
            log_info(optional_logger, "This is the id correlational: ", caughtPokemonMessage->id_correlational);
            log_info(optional_logger, "This is the id message: ", caughtPokemonMessage->id_message);
            break;
        case GET_POKEMON:;

            get_pokemon* getPokemonMessage = stream_to_get_pokemon(stream); 

            log_info(optional_logger, "Get pokemon!");
            log_info(optional_logger, "This is the pokemon: ", getPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the id message: ", getPokemonMessage->id_message);
            break;
        case LOCALIZED_POKEMON:;

            localized_pokemon* localizedPokemonMessage = stream_to_localized_pokemon(stream);

            log_info(optional_logger, "Localized pokemon!");
            log_info(optional_logger, "This is the pokemon: ", localizedPokemonMessage->pokemon); 
            log_info(optional_logger, "This is the size of the list of positions: ", (*localizedPokemonMessage->positions).elements_count);
            log_info(optional_logger, "This is the id correlational: ", localizedPokemonMessage->id_correlational);
            log_info(optional_logger, "This is the id message: ", localizedPokemonMessage->id_message);
            break;
        case SUSCRIPTOR:; 

            subscribe* subscribeMessage = stream_to_subscribe(stream);

            log_info(optional_logger, "Subscribe!");
            log_info(optional_logger, "This is the queue: ", subscribeMessage->colaMensajes);            
            break;
        case NEW_CONNECTION:; 

            new_connection* newConnectionMessage = stream_to_new_connection(stream);
            free(newConnectionMessage);
            log_info(optional_logger, "New connection!");
            break;
        case CONNECTION:;

            connection* connectionMessage = stream_to_connection(stream);

            log_info(optional_logger, "Connection!"); 
            log_info(optional_logger, "This is the id connection: ", connectionMessage->id_connection);
            break;
        case RECONNECT:;

            reconnect* reconnectMessage = stream_to_reconnect(stream);

            log_info(optional_logger, "Reconnect!");
            log_info(optional_logger, "This is the id connection: ",reconnectMessage->id_connection);
            break; 
        case ACK:;

            ack* acknowledgementMessage = stream_to_ack(stream);

            log_info(optional_logger, "Acknowledgement!");
            log_info(optional_logger, "This is the id message: ", acknowledgementMessage->id_message);
            break;
        case -1:;
            break;
        default:;
            log_info(optional_logger, "Cannot understand op_code received.");
    }
}
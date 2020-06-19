#include "iniciarGameCard.h"

void receiveMessage(uint32_t cod_op, uint32_t sizeofstruct, uint32_t client_fd) {
	void* stream = malloc(sizeofstruct);
    uint32_t* id_message = malloc(sizeof(uint32_t));
    if (recv(client_fd, stream, sizeofstruct, MSG_WAITALL)<=0){free(stream); return;}

    switch(cod_op){
        case NEW_POKEMON:;
            new_pokemon* newPokemonMessage = stream_to_new_pokemon(stream,id_message,false); 
            log_info(optional_logger, "New pokemon!");
            
            send_ack(client_fd, *id_message);
            //TODO SEND APPEARED
            
            break;
        case CATCH_POKEMON:;
            catch_pokemon* catchPokemonMessage = stream_to_catch_pokemon(stream,id_message,false);
            log_info(optional_logger, "Catch pokemon!");

            send_ack(client_fd, *id_message);
            //TODO SEND CAUGHT
            
            break;
        case GET_POKEMON:;
            get_pokemon* getPokemonMessage = stream_to_get_pokemon(stream,id_message,false);
            log_info(optional_logger, "Get pokemon!"); 

            send_ack(client_fd, *id_message);
            //TODO SEND LOCALIZED
            
            break;
        case CONNECTION:;
            connection* connectionMessage = stream_to_connection(stream);
            id_connection = connectionMessage->id_connection;
            suscribirseA(NEW_POKEMON, client_fd);
            suscribirseA(CATCH_POKEMON, client_fd);
            suscribirseA(GET_POKEMON, client_fd);
            log_info(optional_logger, "Connection!"); 
            log_info(optional_logger, "This is the id connection: %d", connectionMessage->id_connection);
            log_info(optional_logger, "Subscribing to queues %d, %d and & %d", NEW_POKEMON, CATCH_POKEMON, GET_POKEMON);
            break;
        
    }
}

void iniciarGameCard(){

    config = config_create("./cfg/gamecard.config");
    uint32_t showConsole = config_get_int_value(config,"LOG_SHOW");
    obligatory_logger = log_create("./cfg/obligatory.log", "obligatory", showConsole, LOG_LEVEL_INFO); 
    optional_logger = log_create("./cfg/optional.log", "optional", showConsole, LOG_LEVEL_INFO);
    char* IP_GAMECARD = config_get_string_value(config,"IP_GAMECARD");
    char* PUERTO_GAMECARD = config_get_string_value(config,"PUERTO_GAMECARD");
    PUERTO_BROKER = config_get_string_value(config,"PUERTO_BROKER");
    IP_BROKER = config_get_string_value(config,"IP_BROKER");

    iniciarTallGrass();
    //Se intentara suscribir globalmente al Broker a las siguientes colas de mensajes
    //TODO
    
    suscribirseATodo();

    start_server(IP_GAMECARD,PUERTO_GAMECARD,request);

    pthread_join(suscripcionNewPokemon,NULL);
    pthread_join(suscripcionCatchPokemon,NULL);
    pthread_join(suscripcionGetPokemon,NULL);

    finalizarGameCard();
}

void suscribirseATodo(){
    uint32_t socket_new_pokemon = crear_conexion(IP_BROKER,PUERTO_BROKER);
    crearSuscripcion(socket_new_pokemon, NEW_POKEMON);

    uint32_t socket_catch_pokemon = crear_conexion(IP_BROKER,PUERTO_BROKER);
    crearSuscripcion(socket_new_pokemon, CATCH_POKEMON);

    uint32_t socket_get_pokemon = crear_conexion(IP_BROKER,PUERTO_BROKER);
    crearSuscripcion(socket_get_pokemon, GET_POKEMON);

    /*
    send_new_connection(socket_broker);
    args_pthread arguments; 
    arguments.codigoCola = NEW_POKEMON;
    arguments.socket = socket_broker;
    pthread_create(&suscripcionNewPokemon,NULL,(void*)suscribirseA, (void *)&arguments);
    socket_broker = crear_conexion(IP_BROKER,PUERTO_BROKER);
    args_pthread arguments2; 
    arguments2.codigoCola = CATCH_POKEMON;
    arguments2.socket = socket_broker;
    pthread_create(&suscripcionCatchPokemon,NULL,(void*)suscribirseA,(void *)&arguments2);
    socket_broker = crear_conexion(IP_BROKER,PUERTO_BROKER);
    args_pthread arguments3; 
    arguments3.codigoCola = GET_POKEMON;
    arguments3.socket = socket_broker;
    pthread_create(&suscripcionGetPokemon,NULL,(void*)suscribirseA,(void *)&arguments3);
    connect_client(); */
}

void crearSuscripcion(uint32_t socket,op_code codeOperation){
    //socket = 
    send_new_connection(socket);
    args_pthread* arguments = malloc(sizeof(args_pthread)); 
    arguments->codigoCola = codeOperation;
    arguments->socket = malloc(sizeof(uint32_t));
    *arguments->socket = socket;
    pthread_create(&suscripcionNewPokemon,NULL,(void*)subscribeAndConnect, arguments);
   
}

void subscribeAndConnect(args_pthread* arguments){
    suscribirseA(arguments->codigoCola,*arguments->socket);
    connect_client(*arguments->socket);
}

void connect_client(uint32_t socket){
    on_request request = &receiveMessage; 
    
    t_process_request* process_request = malloc(sizeof(t_process_request)); 
    (*process_request).socket = malloc(sizeof(uint32_t));
    *(*process_request).socket = socket; 
    (*process_request).request_receiver = request;

    id_connection = receive_connection_id(socket);
    while(1){
        serve_client(process_request);
        close(socket);
        socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
        send_reconnect(socket, id_connection);
    } 

    pthread_join(server, NULL);
}

void finalizarGameCard(){
    log_destroy(obligatory_logger);
    log_destroy(optional_logger);
    config_destroy(config);
    close(socket_broker);
}
